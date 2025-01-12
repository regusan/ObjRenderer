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
    class DownSampleData
    {
    public:
        DownSampleData(Vector2i bufferSize, int kernelSize)
        {
            this->bufferSize = bufferSize;
            this->kernelSize = kernelSize;
        }
        Vector2i bufferSize;
        int kernelSize;
    };
    Vector3f SimpleBloom(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y);
    void BloomWithDownSampling(GBuffers &gbuffers, RenderingEnvironmentParameters &environment);
    void ScreenSpaceAmbientOcculusionCryTek(GBuffers &gbuffers, RenderingEnvironmentParameters &environment);
    void ScreenSpaceReflection(GBuffers &gbuffers, RenderingEnvironmentParameters &environment);
    void ScreenSpaceShadow(GBuffers &gbuffers, RenderingEnvironmentParameters &environment);
}