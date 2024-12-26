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
    Vector3f specularColor = Vector3f(1, 0, 0);
    Vector3f directionalLightColor = Vector3f(1, 1, 1);

    Vector3f ref = MathPhysics::Reflect(light0, gbuffers.normalWS.SampleColor(x, y)).normalized();
    float refval = ref.dot(environment.viewMat.row(2).head<3>().normalized());
    float specularScalar = clamp<float>(refval, 0.0, 1.0);
    specularScalar = powf(specularScalar, 40);          // * in.material->specularShapness / 1000.0f;
    Vector3f specular = specularColor * specularScalar; // in.material->specular * specularScalar;

    // return specular + Vector3f(0.3, 0.3, 0.3);
    float directIntencity = gbuffers.normalWS.SampleColor(x, y).dot(light0);
    directIntencity = clamp<float>(directIntencity, 0, 1);
    Vector3f directionalLight = directionalLightColor * directIntencity;

    Vector3f light = directionalLight + specular; // + environment.ambientLight;
    // return light;
    light = light.cwiseMax(Vector3f(0, 0, 0));
    Vector3f finalColor = gbuffers.diffuse.SampleColor(x, y).array() * light.array();
    finalColor = finalColor.cwiseMin(Vector3f(1, 1, 1)).cwiseMax(Vector3f(0, 0, 0));
    return finalColor;
}