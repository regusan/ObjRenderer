#pragma once
#include "../header/ShaderHeader.hpp"
#include "../header/EigenHeader.hpp"
#include "../header/RenderingHeader.hpp"
#include "../header/MathHeader.hpp"
#include "../Rendering/RenderingEnvironmentParameters.hpp"
#include <random>
using namespace std;
using namespace Transform;

namespace PostProcessShader
{
    Vector3f SimpleBloom(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y);
    void BloomWithDownSampling(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, float bloomThreshold = 0.8f, float intencity = 1);
    void BloomWithMultipleConv(GBuffers &gbuffers, RenderingEnvironmentParameters &environment);
    void ScreenSpaceAmbientOcculusionCryTek(GBuffers &gbuffers, RenderingEnvironmentParameters &environment);
    void SSAOPlusSSGI(GBuffers &gbuffers, RenderingEnvironmentParameters &environment);
    void ScreenSpaceReflection(GBuffers &gbuffers, RenderingEnvironmentParameters &environment);
    void ScreenSpaceShadow(GBuffers &gbuffers, RenderingEnvironmentParameters &environment);
    void AutoExposure(GBuffers &gbuffers, RenderingEnvironmentParameters &environment);
}