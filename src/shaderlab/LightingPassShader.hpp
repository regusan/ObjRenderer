#pragma once
#include "../header/EigenHeader.hpp"
#include "../header/RenderingHeader.hpp"
#include "../Math/Math3D.hpp"
#include <random>
using namespace std;
using namespace Transform;

inline const Vector3f DefferedLightingPassShader(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y)
{
    Vector3f light0 = Vector3f(1, 1, 1).normalized();
    float light = gbuffers.normalWS.SampleColor(x, y).dot(light0);
    light = clamp<float>(light, 0, 1);
    return gbuffers.diffuse.SampleColor(x, y) * light;
}