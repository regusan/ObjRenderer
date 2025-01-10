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
    }
}