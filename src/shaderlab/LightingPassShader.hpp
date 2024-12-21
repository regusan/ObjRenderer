#pragma once
#include "../header/EigenHeader.hpp"
#include "../header/RenderingHeader.hpp"
#include "../Math/Math3D.hpp"
#include <random>
using namespace std;
using namespace Transform;

inline const Vector3f DefferedLightingPassShader(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y)
{
    Vector3f light0 = environment.directionalLights.at(0);
    float intencity = gbuffers.normalWS.SampleColor(x, y).dot(light0);
    intencity = clamp<float>(intencity, 0, 1);
    Vector3f light = Vector3f(intencity, intencity, intencity) + environment.ambientLight;
    light = light.cwiseMin(Vector3f(1, 1, 1)).cwiseMax(Vector3f(0, 0, 0));
    return gbuffers.diffuse.SampleColor(x, y).array() * light.array();
}