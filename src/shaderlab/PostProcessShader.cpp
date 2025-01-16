#include "PostProcessShader.hpp"

namespace PostProcessShader
{

    Vector3f SimpleBloom(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y)
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

    void BloomWithDownSampling(GBuffers &gbuffers, RenderingEnvironmentParameters &environment)
    {
        // ダウンサンプリングしたバッファを用意

        vector<RenderTarget::DownSampleData> cinemaDSD = {
            RenderTarget::DownSampleData(gbuffers.screenSize / 1, 3),
            RenderTarget::DownSampleData(gbuffers.screenSize / 2, 5),
            RenderTarget::DownSampleData(gbuffers.screenSize / 4, 9),
            RenderTarget::DownSampleData(gbuffers.screenSize / 8, 17),
            RenderTarget::DownSampleData(gbuffers.screenSize / 16, 33),
            RenderTarget::DownSampleData(gbuffers.screenSize / 32, 33),
            RenderTarget::DownSampleData(gbuffers.screenSize / 64, 33),
        };
        vector<RenderTarget::DownSampleData> midDSD = {
            RenderTarget::DownSampleData(gbuffers.screenSize / 4, 3),
            RenderTarget::DownSampleData(gbuffers.screenSize / 8, 5),
            RenderTarget::DownSampleData(gbuffers.screenSize / 16, 7),
            RenderTarget::DownSampleData(gbuffers.screenSize / 32, 15),
        };
        vector<RenderTarget::DownSampleData> dsd = (environment.quality == RenderingQuality::Cinema) ? cinemaDSD : midDSD;

        RenderTarget bloomSource(gbuffers.screenSize.x(), gbuffers.screenSize.y());
#pragma omp parallel for
        for (int y = 0; y < bloomSource.getScreenSize().y(); y++)
            for (int x = 0; x < bloomSource.getScreenSize().x(); x++)
            {
                Vector3f beauty = gbuffers.beauty.SampleColor(x, y);
                Vector3f overflow = Vector3f(fmax(0, beauty.x() - 1), fmax(0, beauty.y() - 1), fmax(0, beauty.z() - 1));
                bloomSource.PaintPixel(x, y, gbuffers.emission.SampleColor(x, y) + overflow);
            }

        vector<RenderTarget> downSampledBuffers = bloomSource.GausiannBlurWithDownSample(dsd);
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
    void ScreenSpaceAmbientOcculusionCryTek(GBuffers &gbuffers, RenderingEnvironmentParameters &environment)
    {
        int maxSampleNum = (environment.quality == RenderingQuality::Cinema) ? 200 : 10;
        int NoiseCount = maxSampleNum * 10;
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
            uint seed = y + environment.time + 1;
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
                for (int i = 0; i < maxSampleNum; i++)
                {
                    Vector3f randomVS = positionVS + noises[seed % NoiseCount]; // + GeometryMath::RotateVectorToBasis(noises[seed % NoiseCount], -normalVS);
                    seed = GeometryMath::xorshift(seed);
                    const Vector3f randomSS = (randomVS / randomVS.z() + Vector3f(1, 1, 1)) * 0.5;
                    float factDepth = gbuffers.positionVS.SampleColor01(randomSS.x(), randomSS.y()).z();
                    visibleCount += factDepth > randomVS.z();
                }
                float ratio = fmin(1, static_cast<float>(visibleCount) / maxSampleNum * 2);
                gbuffers.AO.PaintPixel(x, y, Vector3f(ratio, ratio, ratio));
            }
        }
        if (environment.quality == RenderingQuality::Cinema)
            gbuffers.AO = gbuffers.AO.GausiannBlur(3);
    }
    void ScreenSpaceReflection(GBuffers &gbuffers, RenderingEnvironmentParameters &environment)
    {
        // 光線を飛ばす回数
        const int maxRayNum = (environment.quality == RenderingQuality::Cinema) ? 200 : 30;
        // レイの最大距離
        const float maxRayLength = 3;
        const float rayLength = maxRayLength / maxRayNum;
        // 自分自身に反射するのを防ぐための最小距離
        const float minimumLength = rayLength * 0;

        float maxThickness = 7.0f / maxRayNum;

#pragma omp parallel for
        for (int y = 0; y < gbuffers.screenSize.y(); y++)
        {
            uint seed = y + environment.time + 1;
            for (int x = 0; x < gbuffers.screenSize.x(); x++)
            {
                // 深度が無限遠だったら処理しない
                if (gbuffers.depth.SampleColor(x, y).x() == numeric_limits<float>::max())
                {
                    continue;
                }

                const Vector3f positionVS = gbuffers.positionVS.SampleColor(x, y);
                Vector3f normalVS = (gbuffers.normalVS.SampleColor(x, y)); //* (1 - roughness) + noises[seed % NoiseCount] * roughness).normalized();
                Vector3f reflect = MathPhysics::Reflect(positionVS, normalVS).normalized();
                // TODO:二分探索にして高速化する
                for (int i = 1; i <= maxRayNum; i++)
                {

                    float noise = seed % 1000 / 1000.0f;
                    const float currentRayLength = rayLength * (i + noise) + minimumLength;
                    seed = GeometryMath::xorshift(seed);
                    const Vector3f offsetVS = reflect * currentRayLength;
                    const Vector3f rayPosVS = positionVS + offsetVS;
                    const Vector3f rayPosSS = (rayPosVS / rayPosVS.z() + Vector3f(1, 1, 1)) * 0.5;
                    // レイが画面外に行ったら強制終了
                    if (rayPosSS.x() < 0 || rayPosSS.x() > 1 || rayPosSS.y() < 0 || rayPosSS.y() > 1)
                        break;

                    // GBufferから取得した実際の深度
                    const float actualDepth = gbuffers.positionVS.SampleColor01(rayPosSS.x(), rayPosSS.y()).z();
                    const float actualBackDepth = gbuffers.backPositionVS.SampleColor01(rayPosSS.x(), rayPosSS.y()).z();
                    // 実際の深度よりレイが奥にあったらヒット判定、かつ厚さが一定以下だったら反射を描画
                    if (rayPosVS.z() > actualDepth && (rayPosVS.z() < actualBackDepth || rayPosVS.z() - actualDepth < maxThickness))
                    {
                        gbuffers.reflection.PaintPixel(x, y, gbuffers.beauty.SampleColor01(rayPosSS.x(), rayPosSS.y()));
                        break;
                    }
                }
            }
        }
        if (environment.quality == RenderingQuality::Cinema)
            gbuffers.reflection = gbuffers.reflection.GausiannBlur(11);
    }
    void ScreenSpaceShadow(GBuffers &gbuffers, RenderingEnvironmentParameters &environment)
    {
        // 光線を飛ばす回数
        const int maxRayNum = (environment.quality == RenderingQuality::Cinema) ? 1000 : 30;
        // レイの最大距離
        const float maxRayLength = (environment.quality == RenderingQuality::Cinema) ? 30 : 10;
        const float rayLength = maxRayLength / maxRayNum;
        // 自分自身に反射するのを防ぐための最小距離
        const float minimumLength = rayLength * 5;

        float maxThickness = 30.0f / maxRayNum;
        maxThickness = 1;

#pragma omp parallel for
        for (int y = 0; y < gbuffers.screenSize.y(); y++)
        {
            uint seed = y + environment.time + 1;
            for (int x = 0; x < gbuffers.screenSize.x(); x++)
            {
                // 深度が無限遠だったら処理しない
                if (gbuffers.depth.SampleColor(x, y).x() == numeric_limits<float>::max())
                {
                    continue;
                }

                const Vector3f positionVS = gbuffers.positionVS.SampleColor(x, y);
                Vector3f lightWS = environment.directionalLights.at(0).direction;
                Vector3f normalVS = (Transform::ResetPosition(ResetScale(environment.viewMat)) * Vector4f(lightWS.x(), lightWS.y(), lightWS.z(), 1)).head<3>().normalized();
                // normalVS = Vector3f(1, 1, -1).normalized();
                //  TODO:二分探索にして高速化する
                for (int i = 1; i <= maxRayNum; i++)
                {

                    float noise = seed % 1000 / 1000.0f;
                    const float currentRayLength = rayLength * (i + noise) + minimumLength;
                    seed = GeometryMath::xorshift(seed);
                    const Vector3f offsetVS = normalVS * currentRayLength;
                    const Vector3f rayPosVS = positionVS + offsetVS;
                    const Vector3f rayPosSS = (rayPosVS / rayPosVS.z() + Vector3f(1, 1, 1)) * 0.5;
                    // レイが画面外に行ったら強制終了
                    if (rayPosSS.x() < 0 || rayPosSS.x() > 1 || rayPosSS.y() < 0 || rayPosSS.y() > 1)
                        break;

                    // GBufferから取得した実際の深度
                    const float actualDepth = gbuffers.positionVS.SampleColor01(rayPosSS.x(), rayPosSS.y()).z();
                    const float actualBackDepth = gbuffers.backPositionVS.SampleColor01(rayPosSS.x(), rayPosSS.y()).z();
                    // 実際の深度よりレイが奥にあったらヒット判定、かつ厚さが一定以下だったら反射を描画
                    if (rayPosVS.z() > actualDepth && (rayPosVS.z() < actualBackDepth || rayPosVS.z() - actualDepth < maxThickness))
                    {
                        gbuffers.SSShadow.PaintPixel(x, y, Vector3f(0, 0, 0));
                        break;
                    }
                }
            }
        }
        if (environment.quality == RenderingQuality::Cinema)
            gbuffers.SSShadow = gbuffers.SSShadow.GausiannBlur(11);
    }
}