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
    float shadowSampled = gbuffers.SSShadow.SampleColor(x, y).x();

    Vector3f cameraPosWS = environment.viewMat.inverse().col(3).head<3>();

    // DirectionalLight関連の情報を取得
    DirectionalLight light0 = environment.directionalLights.at(0);

    // 反射ベクトルの計算
    Vector3f ref = MathPhysics::Reflect(light0.direction, normalWSSampled);
    Vector3f view = (positionWSSampled - cameraPosWS).normalized();
    float refval = ref.dot(view);

    // スペキュラの計算
    float specularScalar = clamp<float>(refval, 0.0, 1.0);
    specularScalar = powf(specularScalar, 10.0f / specularSampled.norm());
    Vector3f specular = specularSampled.normalized() * specularScalar;

    // DirectionalLightによるDiffuseライティング計算
    float directIntencity = max<float>(normalWSSampled.dot(light0.direction), 0);
    Vector3f directionalLight = diffuseSampled.array() * light0.color.array() * directIntencity * aoSampled * shadowSampled;

    // アンビエントライト取得
    Vector3f ambient = environment.ambientLight.array() * diffuseSampled.array() * aoSampled;

    // ライト結果の合成
    Vector3f finalColor = directionalLight + specular + ambient + emissionSampled;

    return finalColor;
}

/// @brief フォンシェーダーのライトパスシェーダー
/// @param gbuffers
/// @param environment
/// @param x
/// @param y
/// @return 該当ピクセルの色
inline const Vector3f FinalLightingPassShader(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y)
{
    Vector3f fogColor = environment.fogColor;
    // 物体がなければフォグカラー
    float depthSampled = gbuffers.depth.SampleColor(x, y).x();
    if (depthSampled == numeric_limits<float>::max())
        return fogColor;
    Vector3f finalColor = gbuffers.beauty.SampleColor(x, y);
    if (environment.quality < RenderingQuality::Low)
    {
        // TODO:強度のみの簡易版
        Vector3f spec = gbuffers.specular.SampleColor(x, y);
        Vector3f finalColor = gbuffers.reflection.SampleColor(x, y).array() * spec.array() +
                              finalColor.array() * (Vector3f(1, 1, 1) - spec).array();
    }
    float depthFogRatio = clamp<float>((depthSampled - environment.fogNearFar.x()) / (environment.fogNearFar.y() - environment.fogNearFar.x()), 0, 1);
    finalColor = finalColor * (1 - depthFogRatio) + fogColor * depthFogRatio;
    return finalColor;
}
