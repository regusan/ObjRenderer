#pragma once
#include "../header/EigenHeader.hpp"
#include "../header/RenderingHeader.hpp"
#include "../Math/Math3D.hpp"
#include <random>
using namespace std;
using namespace Transform;

namespace LighingShader
{
    inline Vector3f SampleFromMipMap(vector<RenderTarget> &mipmap, const float level, const Vector2f uv)
    {
        int index = clamp<float>(level * mipmap.size(), 0, mipmap.size() - 1);
        return mipmap[index].SampleColor01BiLinear(uv.x(), uv.y());
    }

    inline const Vector3f PBRLighingShader(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y)
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
        // Vector3f specularSampled = gbuffers.specular.SampleColor(x, y);
        Vector3f diffuseSampled = gbuffers.diffuse.SampleColor(x, y);
        Vector3f ORM = gbuffers.ORM.SampleColor(x, y);
        float bakedOcclusionSampled = ORM.x();
        float roughnessSampled = ORM.y();
        float metalicSampled = ORM.z();
        Vector3f normalWSSampled = gbuffers.normalWS.SampleColor(x, y);
        float aoSampled = gbuffers.AO.SampleColor(x, y).x();
        float shadowSampled = gbuffers.SSShadow.SampleColor(x, y).x();
        Vector3f irradianceSampled = gbuffers.irradiance.SampleColor(x, y);
        // Vector2f sampledUV = gbuffers.uv.SampleColor(x, y).head<2>();

        // 各種ベクトル
        Vector3f reflectEyeN = MathPhysics::Reflect(view, normalWSSampled);
        float VDotN = view.dot(normalWSSampled);

        // スカイスフィアからスペキュラをサンプル
        Vector2f skyUV = TextureMath::UVMod1(GeometryMath::DirVec2SphereUV(normalWSSampled) + environment.skySphereOffset);
        Vector2f skyUVRef = TextureMath::UVMod1(GeometryMath::DirVec2SphereUV(reflectEyeN) + environment.skySphereOffset);
        Vector3f skySpecSampled = Vector3f::Ones();
        if (environment.skySphere)
            skySpecSampled = SampleFromMipMap(environment.skyMipmap, roughnessSampled, skyUVRef);
        Vector3f skyAmbient = Vector3f::Ones();
        if (environment.skySphere)
            environment.skyMipmap[environment.skyMipmap.size() - 1].SampleColor01BiLinear(skyUV.x(), skyUV.y());

        // DirectionalLight関連の情報を取得
        DirectionalLight light0 = environment.directionalLights.at(0);
        // Vector3f lightRefDir = MathPhysics::Reflect(light0.direction, normalWSSampled);
        float directIntencity = max<float>(normalWSSampled.dot(light0.direction), 0);
        float Li = aoSampled * bakedOcclusionSampled * shadowSampled * directIntencity;

        // 直接光によるライティング計算
        Vector3f DiffuseBRDF = light0.color.array() * Li;
        DiffuseBRDF = DiffuseBRDF + skySpecSampled * (1 - roughnessSampled) * (1 - fabs(VDotN));

        // スペキュラ計算
        Vector3f specularBRDF = diffuseSampled.array() * skySpecSampled.array() * Li;

        // 環境光によるライティング計算
        Vector3f ambient = diffuseSampled.array() * skyAmbient.array() * environment.ambientLight.array();

        return DiffuseBRDF * (1.0f - metalicSampled) + specularBRDF * metalicSampled + ambient + irradianceSampled;
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
            if (environment.skySphere.has_value())
            {
                Vector2f posNormed = Vector2f((float)x / gbuffers.screenSize.x() * 2 - 1, (float)y / gbuffers.screenSize.y() * 2 - 1);
                Matrix4f viewNormal = Transform::ResetScale(Transform::ResetPosition(environment.viewMat.inverse()));
                Vector3f normedView = (viewNormal * Vector4f(posNormed.x(), posNormed.y(), 1, 1)).head<3>().normalized();
                Vector2f skyUV = TextureMath::UVMod1(GeometryMath::DirVec2SphereUV(normedView) + environment.skySphereOffset);
                return environment.skySphere->SampleColor01BiLinear(skyUV.x(), skyUV.y());
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
}