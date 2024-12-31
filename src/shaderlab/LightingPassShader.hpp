#pragma once
#include "../header/EigenHeader.hpp"
#include "../header/RenderingHeader.hpp"
#include "../Math/Math3D.hpp"
#include <random>
using namespace std;
using namespace Transform;

/// @brief フォンシェーダーのライトパスシェーダー
/// @param gbuffers
/// @param environment
/// @param x
/// @param y
/// @return 該当ピクセルの色
inline const Vector3f DefferedLightingPassShader(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y)
{
    Vector3f light0 = environment.directionalLights.at(0);
    Vector3f directionalLightColor = Vector3f(1, 1, 1);

    Vector3f ref = -MathPhysics::Reflect(light0, gbuffers.normalWS.SampleColor(x, y));
    float refval = ref.dot(environment.viewMat.row(2).head<3>().normalized());
    float specularScalar = clamp<float>(refval, 0.0, 1.0);
    specularScalar = powf(specularScalar, 1000.0f / gbuffers.specular.SampleColor(x, y).norm());
    Vector3f specular = gbuffers.specular.SampleColor(x, y).normalized() * specularScalar;

    // return specular + Vector3f(0.3, 0.3, 0.3);
    float directIntencity = max<float>(gbuffers.normalWS.SampleColor(x, y).dot(light0), 0);
    Vector3f directionalLight = gbuffers.diffuse.SampleColor(x, y).array() * directionalLightColor.array() * directIntencity;

    // light = light.normalized();
    Vector3f ambient = environment.ambientLight;

    Vector3f finalColor = directionalLight + specular + ambient + gbuffers.emission.SampleColor(x, y);
    return finalColor;
}