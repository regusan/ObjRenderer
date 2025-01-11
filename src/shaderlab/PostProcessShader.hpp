#pragma once
#include "../header/ShaderHeader.hpp"
#include "../header/EigenHeader.hpp"
#include "../header/RenderingHeader.hpp"
#include "../TransformMat.hpp"
#include "../Math/Math3D.hpp"
#include <random>
using namespace std;
using namespace Transform;

namespace PostProcessShader
{
    inline const Vector3f SimpleBloom(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y)
    {
        int kernelSize = 12;
        int kernelScale = 8;
        Vector3f sum = Vector3f(0, 0, 0);
        for (int lx = -kernelSize / 2; lx < kernelSize / 2; lx++)
        {
            for (int ly = -kernelSize / 2; ly < kernelSize / 2; ly++)
            {
                sum += gbuffers.emission.SampleColor(x + lx * kernelScale, y + ly * kernelScale);
            }
        }

        sum /= kernelSize * kernelSize;
        return sum + gbuffers.beauty.SampleColor(x, y);
    }

    void BloomWithDownSampling(GBuffers &gbuffers)
    {
        // ダウンサンプリングしたバッファを用意

        vector<Vector2i> downSampleSizes = {
            // gbuffers.screenSize / 2,
            gbuffers.screenSize / 4,
            gbuffers.screenSize / 8,
            gbuffers.screenSize / 16,
            gbuffers.screenSize / 32,
        };
        vector<int> kernelSizes = {
            // 3,
            3,
            5,
            7,
            15,
        };
        RenderTarget bloomSource(gbuffers.screenSize.x(), gbuffers.screenSize.y());
#pragma omp parallel for
        for (int y = 0; y < bloomSource.getScreenSize().y(); y++)
            for (int x = 0; x < bloomSource.getScreenSize().x(); x++)
            {
                Vector3f beauty = gbuffers.beauty.SampleColor(x, y);
                Vector3f overflow = Vector3f(fmax(0, beauty.x() - 1), fmax(0, beauty.y() - 1), fmax(0, beauty.z() - 1));
                bloomSource.PaintPixel(x, y, gbuffers.emission.SampleColor(x, y) + overflow);
            }

        vector<RenderTarget> downSampledBuffers(downSampleSizes.size());
#pragma omp parallel for
        for (size_t i = 0; i < downSampleSizes.size(); i++)
        {
            downSampledBuffers[i] = bloomSource.DownSample(downSampleSizes[i]).GausiannBlur(kernelSizes[i]).UpSample(gbuffers.screenSize);
        }
#pragma omp parallel for
        for (int y = 0; y < bloomSource.getScreenSize().y(); y++)
            for (int x = 0; x < bloomSource.getScreenSize().x(); x++)
            {
                // ダウンサンプリングしたバッファを合成
                Vector3f sum = Vector3f(0, 0, 0);
                for (size_t i = 0; i < downSampledBuffers.size(); i++)
                    sum += downSampledBuffers[i].SampleColor(x, y);
                gbuffers.beauty.PaintPixel(x, y, sum + gbuffers.beauty.SampleColor(x, y));
            }
    }
    void ScreenSpaceAmbientOcculusionCryTek(GBuffers &gbuffers)
    {
        int N = 50;
        int NoiseCount = N * 10;
        int skipSize = 1;
        float sphereRadius = 0.001 * gbuffers.screenSize.x(); // 解像度1000で半径1
        // ノイズを事前計算
        vector<Vector3f> noises;
        uint precomputeSeed = 1;
        for (int i = 0; i < NoiseCount; i++)
        {
            noises.push_back(GeometryMath::MakeRandomPointInSphereByUVSeed(Vector3f(0, 0, 0), sphereRadius, precomputeSeed));
            precomputeSeed = GeometryMath::xorshift(precomputeSeed);
        }

#pragma omp parallel for
        for (int y = 0; y < gbuffers.depth.getScreenSize().y(); y += skipSize)
        {
            uint seed = y;
            for (int x = 0; x < gbuffers.depth.getScreenSize().x(); x += skipSize)
            {
                // 深度が無限遠だったら処理しない
                if (gbuffers.depth.SampleColor(x, y).x() == numeric_limits<float>::max())
                {
                    gbuffers.reflection.PaintPixel(x, y, Vector3f(0, 0, 0));
                    continue;
                }
                int visibleCount = 0;
                Vector3f positionVS = gbuffers.positionVS.SampleColor(x, y);
                Vector3f normalVS = gbuffers.normalVS.SampleColor(x, y);
                for (int i = 0; i < N; i++)
                {
                    Vector3f randomVS = positionVS + noises[seed % NoiseCount]; // + GeometryMath::RotateVectorToBasis(noises[seed % NoiseCount], -normalVS);
                    seed = GeometryMath::xorshift(seed);
                    const Vector3f randomSS = (randomVS / randomVS.z() + Vector3f(1, 1, 1)) * 0.5;
                    float factDepth = gbuffers.positionVS.SampleColor01(randomSS.x(), randomSS.y()).z();
                    visibleCount += factDepth > randomVS.z();
                }
                float ratio = fmin(1, static_cast<float>(visibleCount) / N * 2);
                gbuffers.AO.PaintPixel(x, y, Vector3f(ratio, ratio, ratio));
            }
        }
        gbuffers.AO = gbuffers.AO.GausiannBlur(7);
    }
    void ScreenSpaceReflection(GBuffers &gbuffers)
    {
        // 光線を飛ばす回数
        const int maxRayNum = 30;
        // レイの最大距離
        const float maxRayLength = 3;
        const float rayLength = maxRayLength / maxRayNum;
        // 自分自身に反射するのを防ぐための最小距離
        const float minimumLength = rayLength * 0;

        float maxThickness = 7.0f / maxRayNum;

#pragma omp parallel for
        for (int y = 0; y < gbuffers.screenSize.y(); y++)
        {
            for (int x = 0; x < gbuffers.screenSize.x(); x++)
            {
                // 深度が無限遠だったら処理しない
                if (gbuffers.depth.SampleColor(x, y).x() == numeric_limits<float>::max())
                {
                    gbuffers.reflection.PaintPixel(x, y, Vector3f(0, 0, 0));
                    continue;
                }

                const Vector3f positionVS = gbuffers.positionVS.SampleColor(x, y);
                Vector3f normalVS = gbuffers.normalVS.SampleColor(x, y);
                Vector3f reflect = MathPhysics::Reflect(positionVS, normalVS).normalized();
                // TODO:二分探索にして高速化する
                for (int i = 1; i <= maxRayNum; i++)
                {
                    const float currentRayLength = rayLength * i + minimumLength;
                    const Vector3f offsetVS = reflect * currentRayLength;
                    const Vector3f rayPosVS = positionVS + offsetVS;
                    const Vector3f rayPosSS = (rayPosVS / rayPosVS.z() + Vector3f(1, 1, 1)) * 0.5;
                    // レイが画面外に行ったら強制終了
                    if (rayPosSS.x() < 0 || rayPosSS.x() > 1 || rayPosSS.y() < 0 || rayPosSS.y() > 1)
                        break;

                    // GBufferから取得した実際の深度
                    const float actualDepth = gbuffers.positionVS.SampleColor01(rayPosSS.x(), rayPosSS.y()).z();
                    // 実際の深度よりレイが奥にあったらヒット判定、かつ厚さが一定以下だったら反射を描画
                    if (rayPosVS.z() > actualDepth && rayPosVS.z() - actualDepth < maxThickness)
                    {
                        gbuffers.reflection.PaintPixel(x, y, gbuffers.beauty.SampleColor01(rayPosSS.x(), rayPosSS.y()));
                        break;
                    }
                }
            }
        }
        // SSRの結果を合成
#pragma omp parallel for
        for (int y = 0; y < gbuffers.screenSize.y(); y++)
        {
            for (int x = 0; x < gbuffers.screenSize.x(); x++)
            {
                // TODO:強度のみの簡易版
                Vector3f spec = gbuffers.specular.SampleColor(x, y);
                Vector3f col = gbuffers.reflection.SampleColor(x, y).array() * spec.array() +
                               gbuffers.beauty.SampleColor(x, y).array() * (Vector3f(1, 1, 1) - spec).array();
                gbuffers.beauty.PaintPixel(x, y, col);
            }
        }
    }
}