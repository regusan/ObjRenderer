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
        vector<RenderTarget> downSampledBuffers;
        downSampledBuffers.push_back(gbuffers.emission.DownSample(gbuffers.screenSize / 4).BoxBlur(4).UpSample(gbuffers.screenSize));
        downSampledBuffers.push_back(gbuffers.emission.DownSample(gbuffers.screenSize / 8).BoxBlur(4).UpSample(gbuffers.screenSize));
        downSampledBuffers.push_back(gbuffers.emission.DownSample(gbuffers.screenSize / 32).BoxBlur(3).UpSample(gbuffers.screenSize));
        downSampledBuffers.push_back(gbuffers.emission.DownSample(gbuffers.screenSize / 64).BoxBlur(3).UpSample(gbuffers.screenSize));
        downSampledBuffers.push_back(gbuffers.emission.DownSample(gbuffers.screenSize / 128).BoxBlur(3).UpSample(gbuffers.screenSize));
#pragma omp parallel for
        for (int y = 0; y < gbuffers.emission.getScreenSize().y(); y++)
            for (int x = 0; x < gbuffers.emission.getScreenSize().x(); x++)
            {
                // ダウンサンプリングしたバッファを合成
                Vector3f sum = Vector3f(0, 0, 0);
                for (size_t i = 0; i < downSampledBuffers.size(); i++)
                    sum += downSampledBuffers[i].SampleColor(x, y);

                gbuffers.beauty.PaintPixel(x, y, gbuffers.beauty.SampleColor(x, y) + sum / downSampledBuffers.size());
            }
        }
    void ScreenSpaceAmbientOcculusionCryTek(GBuffers &gbuffers)
    {
        int N = 30;
        int NoiseCount = N * 10;
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
        for (int y = 0; y < gbuffers.depth.getScreenSize().y(); y++)
        {
            uint seed = y;
            for (int x = 0; x < gbuffers.depth.getScreenSize().x(); x++)
            {
                int visibleCount = 0;
                Vector3f positionVS = gbuffers.positionVS.SampleColor(x, y);
                for (int i = 0; i < N; i++)
                {
                    Vector3f randomVS = positionVS + noises[seed % NoiseCount];
                    seed = GeometryMath::xorshift(seed);
                    const Vector3f randomSS = (randomVS / randomVS.z() + Vector3f(1, 1, 1)) * 0.5;
                    float factDepth = gbuffers.positionVS.SampleColor01(randomSS.x(), randomSS.y()).z();
                    visibleCount += factDepth > randomVS.z();
                }
                float ratio = static_cast<float>(visibleCount) / N;
                gbuffers.AO.PaintPixel(x, y, Vector3f(ratio, ratio, ratio));
            }
        }
        gbuffers.AO = gbuffers.AO.BoxBlur(5);
    }
}