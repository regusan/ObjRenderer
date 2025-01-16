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
    {
        return Vector3f(0, 0, 0);
    }

    Vector3f positionWSSampled = gbuffers.positionWS.SampleColor(x, y);
    Vector3f cameraPosWS = environment.viewMat.inverse().col(3).head<3>();
    Vector3f view = (positionWSSampled - cameraPosWS).normalized();

    // バッファからのサンプル結果
    Vector3f specularSampled = gbuffers.specular.SampleColor(x, y);
    Vector3f diffuseSampled = gbuffers.diffuse.SampleColor(x, y);
    Vector3f normalWSSampled = gbuffers.normalWS.SampleColor(x, y);
    float aoSampled = gbuffers.AO.SampleColor(x, y).x();
    float shadowSampled = gbuffers.SSShadow.SampleColor(x, y).x();
    Vector3f skyDiffSampled = Vector3f(0, 0, 0);
    Vector2f diffSkyUV = Vector2f(0, 0);
    if (environment.skySphereDiffuse.has_value())
    {
        diffSkyUV = TextureMath::UVMod1(GeometryMath::DirVec2SphereUV(normalWSSampled) + environment.skySphereOffset);
        skyDiffSampled = environment.skySphereDiffuse->SampleColor01(diffSkyUV.x(), diffSkyUV.y()).array();
    }
    // DirectionalLight関連の情報を取得
    DirectionalLight light0 = environment.directionalLights.at(0);

    // DirectionalLightによるDiffuseライティング計算
    float directIntencity = max<float>(normalWSSampled.dot(light0.direction), 0);
    float Li = aoSampled * shadowSampled;
    Vector3f DiffuseBRDFInIllum = light0.color;
    Vector3f DiffuseBRDF = diffuseSampled.array() * light0.color.array();
    if (environment.skySphereDiffuse.has_value())
    {
        DiffuseBRDFInIllum = DiffuseBRDFInIllum.array() * skyDiffSampled.array(); // ディフーズ入力にスカイスフィアを考慮
        Li *= DiffuseBRDFInIllum.norm();
        DiffuseBRDF = DiffuseBRDF.array() * DiffuseBRDFInIllum.array() * skyDiffSampled.array();
    }
    else
    {
        Li *= directIntencity; // 平行光源を考慮
        DiffuseBRDF = DiffuseBRDFInIllum * Li;
    }

    // 反射ベクトルの計算
    Vector3f specularBRDF;
    Vector3f ref = MathPhysics::Reflect(light0.direction, normalWSSampled);

    // アンビエントライト取得
    Vector3f ambient = environment.ambientLight * aoSampled;
    if (environment.skySphereDiffuse.has_value())
    {
        ambient = ambient.array() * environment.skySphereDiffuse->SampleColor01(diffSkyUV.x(), diffSkyUV.y()).array();
    }
    else
    {
        ambient = ambient.array() * diffuseSampled.array();
    }

    // スペキュラの計算
    // スペキュラマップがあったら
    float roughness = clamp<float>(1.0f - specularSampled.norm() * 0.57735026919, 0, 1); // 1/sqrt(3)=0.57735026919
    if (environment.skySphereSpecular.has_value())
    {
        Vector3f viewRefNorm = MathPhysics::Reflect(view, normalWSSampled);
        Vector2f specSkyUV = TextureMath::UVMod1(GeometryMath::DirVec2SphereUV(viewRefNorm) + environment.skySphereOffset);
        Vector3f envSpecular = environment.skySphereSpecular->SampleColor01(specSkyUV.x(), specSkyUV.y());
        specularBRDF = envSpecular.array() * (environment.ambientLight + Vector3f(directIntencity, directIntencity, directIntencity) * aoSampled * shadowSampled).array();
        gbuffers.reflection.PaintPixel(x, y, envSpecular);
    }
    else
    {
        float refval = ref.dot(view);
        float specularScalar = clamp<float>(refval, 0.0, 1.0);
        specularScalar = powf(specularScalar, 10.0f / specularSampled.norm());
        specularBRDF = specularSampled.normalized() * specularScalar;
    }
    // ライト結果の合成
    Vector3f finalColor = DiffuseBRDF * roughness + specularBRDF * (1 - roughness) + ambient;

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

    // 無限遠だったら
    if (depthSampled == numeric_limits<float>::max())
    {
        // スカイテクスチャが設定されてたらスカイボックス描画
        if (environment.skySphereSpecular.has_value())
        {
            Vector2f posNormed = Vector2f((float)x / gbuffers.screenSize.x() * 2 - 1, (float)y / gbuffers.screenSize.y() * 2 - 1);
            Matrix4f viewNormal = Transform::ResetScale(Transform::ResetPosition(environment.viewMat.inverse()));
            Vector3f normedView = (viewNormal * Vector4f(posNormed.x(), posNormed.y(), 1, 1)).head<3>().normalized();
            Vector2f skyUV = TextureMath::UVMod1(GeometryMath::DirVec2SphereUV(normedView) + environment.skySphereOffset);
            return environment.skySphereSpecular->SampleColor01(skyUV.x(), skyUV.y());
        }
        else // スカイテクスチャがないのでフォグ描画
        {
            return fogColor;
        }
    }
    else
    {
        Vector3f finalColor = gbuffers.beauty.SampleColor(x, y);

        // TODO:強度のみの簡易版
        Vector3f spec = gbuffers.specular.SampleColor(x, y) * 0.5; // TODO:無意味な定数！！
        finalColor = gbuffers.reflection.SampleColor(x, y).array() * spec.array() + finalColor.array() * (Vector3f(1, 1, 1) - spec).array();

        float depthFogRatio = clamp<float>((depthSampled - environment.fogNearFar.x()) / (environment.fogNearFar.y() - environment.fogNearFar.x()), 0, 1);
        finalColor = finalColor * (1 - depthFogRatio) + fogColor * depthFogRatio;
        return finalColor;
    }
}
