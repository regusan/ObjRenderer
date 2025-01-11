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
    // 物体がなければ何もしない
    float depthSampled = gbuffers.depth.SampleColor(x, y).x();
    if (depthSampled == numeric_limits<float>::max())
        return Vector3f(0, 0, 0);
    // バッファからのサンプル結果
    Vector3f specularSampled = gbuffers.specular.SampleColor(x, y);
    Vector3f diffuseSampled = gbuffers.diffuse.SampleColor(x, y);
    Vector3f normalWSSampled = gbuffers.normalWS.SampleColor(x, y);
    Vector3f positionWSSampled = gbuffers.positionWS.SampleColor(x, y);
    Vector3f emissionSampled = gbuffers.emission.SampleColor(x, y);
    float aoSampled = gbuffers.AO.SampleColor(x, y).x();

    Vector3f cameraPosWS = environment.viewMat.inverse().col(3).head<3>();

    // DirectionalLight関連の情報を取得
    DirectionalLight light0 = environment.directionalLights.at(0);
    Vector3f directionalLightColor = Vector3f(.8, .8, .8);

    // 反射ベクトルの計算
    Vector3f ref = MathPhysics::Reflect(light0.direction, normalWSSampled);
    Vector3f view = (positionWSSampled - cameraPosWS).normalized();
    float refval = ref.dot(view);

    // スペキュラの計算
    float specularScalar = clamp<float>(refval, 0.0, 1.0);
    specularScalar = powf(specularScalar, 100.0f / specularSampled.norm());
    Vector3f specular = specularSampled.normalized() * specularScalar;

    // DirectionalLightによるDiffuseライティング計算
    float directIntencity = max<float>(normalWSSampled.dot(light0.direction), 0);
    Vector3f directionalLight = diffuseSampled.array() * light0.color.array() * directIntencity * aoSampled;

    // アンビエントライト取得
    Vector3f ambient = environment.ambientLight.array() * diffuseSampled.array() * aoSampled;

    // ライト結果の合成
    Vector3f finalColor = directionalLight + specular + ambient + emissionSampled;
    return finalColor;
}