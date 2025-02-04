#pragma once
#include "../header/ShaderHeader.hpp"
#include "../header/RenderingHeader.hpp"
#include "../header/EigenHeader.hpp"
#include "../header/MathHeader.hpp"
#include <random>
using namespace std;
using namespace Transform;

namespace RenderingPass
{
    void ExecPostProcessPass(
        GBuffers &gbuffers,
        const Vector3f (*postProcessPass)(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y),
        RenderingEnvironmentParameters &environment);

}