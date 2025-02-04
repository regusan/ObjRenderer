#pragma once
#include "../header/EigenHeader.hpp"
#include "../header/RenderingHeader.hpp"
#include "../Math/Math3D.hpp"
#include <random>
using namespace std;
using namespace Transform;

namespace LighingShader
{
    struct BRDFResult
    {
        float diffuse = 0;
        float specular = 0;
    };

    BRDFResult ReguBRDF(const Vector3f &L, const Vector3f &V, const Vector3f &N)
    {
        BRDFResult retval;
        retval.diffuse = M_1_PI; // 半球状に均等に分布
        retval.specular;
        return retval;
    }

    inline float Fresnel(const float _F0, const float VDotN)
    {
        return clamp<float>(_F0 + (1.0f - _F0) * pow(1.0f - VDotN, 5), 0, 1);
    }
    inline Vector3f Fresnel(const Vector3f _F0, const float VDotN)
    {
        Vector3f f = _F0 + (Vector3f::Ones() - _F0) * pow(1.0f - VDotN, 5);
        return StandardMath::clampv(f, .0f, 1.0f);
    }
    inline Vector3f SampleFromMipMap(vector<RenderTarget> &mipmap, const float level, const Vector2f uv)
    {
        int index = clamp<float>(level * mipmap.size(), 0, mipmap.size() - 1);
        return mipmap[index].SampleColor01BiLinear(uv.x(), uv.y());
    }

    /// @brief 天球を用いたライティング
    /// @param gbuffers
    /// @param environment
    /// @param x
    /// @param y
    /// @return
    inline const Vector3f IBLShader(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y)
    {
        constexpr float irradianceStrength = 10;

        // 物体がなければ何もしない
        float depthSampled = gbuffers.depth.SampleColor(x, y).x();

        if (depthSampled == numeric_limits<float>::max())
        {
            return Vector3f(0, 0, 0);
        }

        // バッファからのサンプル結果
        // Vector3f specularSampled = gbuffers.specular.SampleColor(x, y);
        Vector3f positionWSSampled = gbuffers.positionWS.SampleColor(x, y);
        Vector3f positionVSSampled = gbuffers.positionVS.SampleColor(x, y);
        Vector3f diffuseSampled = gbuffers.diffuse.SampleColor(x, y);
        Vector3f ORM = gbuffers.ORM.SampleColor(x, y);
        float bakedOcclusionSampled = ORM.x();
        float roughnessSampled = ORM.y();
        float metalicSampled = ORM.z();
        Vector3f normalWSSampled = gbuffers.normalWS.SampleColor(x, y);
        Vector3f normalVSSampled = gbuffers.normalVS.SampleColor(x, y);
        float aoSampled = gbuffers.AO.SampleColor(x, y).x();
        float shadowSampled = gbuffers.SSShadow.SampleColor(x, y).x();
        Vector3f irradianceSampled = gbuffers.irradiance.SampleColor(x, y);
        // Vector2f sampledUV = gbuffers.uv.SampleColor(x, y).head<2>();
        // float _F0 = clamp<float>(metalicSampled, 0.03, 1);

        Vector3f emission = Vector3f(fmax(diffuseSampled.x() - 1, 0), fmax(diffuseSampled.y() - 1, 0), fmax(diffuseSampled.z() - 1, 0));

        // 各種ベクトル
        Vector3f cameraPosWS = environment.viewMat.inverse().col(3).head<3>();
        Vector3f view = (positionWSSampled - cameraPosWS).normalized();
        Vector3f reflectEyeN = MathPhysics::Reflect(view, normalWSSampled);
        // float VDotN = -view.dot(normalWSSampled);

        // スカイスフィアからスペキュラをサンプル
        Vector2f skyUV = TextureMath::UVMod1(TextureMath::RectangularToPolarUV(normalWSSampled) + environment.skySphereOffset);
        Vector2f skyUVRef = TextureMath::UVMod1(TextureMath::RectangularToPolarUV(reflectEyeN) + environment.skySphereOffset);
        Vector3f specularSampled, skyDiffSampled, skyAmbient;
        if (environment.skySphere)
        {

            specularSampled = SampleFromMipMap(environment.skyMipmap, roughnessSampled, skyUVRef).array() * environment.directionalLights[0].color.array();
            skyAmbient = environment.skyMipmap[environment.skyMipmap.size() - 1].SampleColor01BiLinear(skyUV.x(), skyUV.y());
            skyDiffSampled = SampleFromMipMap(environment.skyMipmap, roughnessSampled, skyUV).array() * environment.directionalLights[0].color.array();
        }
        else
        {
            specularSampled = skyDiffSampled = skyAmbient = Vector3f::Ones();
        }

        // ローカル反射の結果が存在したらそっちを使う。
        Vector3f screenSpaceReflectionLevel = gbuffers.reflectionLevel.SampleColor(x, y);
        if (screenSpaceReflectionLevel != Vector3f::Zero())
        {
            specularSampled = gbuffers.reflection.SampleColor(x, y) * screenSpaceReflectionLevel.x() +
                              specularSampled * (1.0f - screenSpaceReflectionLevel.x());
        }

        // DirectionalLight関連の情報を取得
        DirectionalLight light0 = environment.directionalLights.at(0);
        // Vector3f lightRefDir = MathPhysics::Reflect(light0.direction, normalWSSampled);
        // float directIntencity = max<float>(normalWSSampled.dot(light0.direction), 0);
        float Li = aoSampled * bakedOcclusionSampled * shadowSampled + environment.ambientLight.norm(); //* directIntencity;

        // 直接光によるライティング計算
        // Vector3f DiffuseBRDF = diffuseSampled.array() * light0.color.array() * Li;
        //        DiffuseBRDF = DiffuseBRDF + skySpecSampled * (1 - roughnessSampled);
        Vector3f DiffuseBRDF = diffuseSampled.array() * skyAmbient.array() * light0.color.array() * Li + emission.array();

        // return Vector3f(fresnel, fresnel, fresnel);
        // スペキュラ計算
        Vector3f specularBRDF = specularSampled.array() * StandardMath::lerp<Vector3f>(roughnessSampled, Vector3f::Ones(), diffuseSampled).array() * Li;

        // 環境光によるライティング計算
        // Vector3f ambient = diffuseSampled.array() * skyAmbient.array() * environment.ambientLight.array();

        // 照明計算

        Vector3f radiation = Vector3f::Zero();

        if (gbuffers.lightBackDepth.SampleColor(x, y).z() > depthSampled && depthSampled > gbuffers.lightDepth.SampleColor(x, y).z())
        {
            Vector3f lightPos = gbuffers.lightPositionVS.SampleColor(x, y);
            float dist = (positionVSSampled - lightPos).norm();
            float norm = normalVSSampled.dot((lightPos - positionVSSampled).normalized());
            float str = clamp<float>(fabs(norm) * (1.5 - dist), 0, 1);
            radiation = gbuffers.lightDomain.SampleColor(x, y) * str;
        }

        Vector3f final = DiffuseBRDF * (1.0f - metalicSampled) + specularBRDF + radiation + irradianceSampled * irradianceStrength;
        // gbuffers.reflection.PaintPixel(x, y, final);
        return final;
    }

    /// @brief フォンシェーダーのライトパスシェーダー
    /// @param gbuffers
    /// @param environment
    /// @param x
    /// @param y
    /// @return 該当ピクセルの色
    inline const Vector3f BackGroundLighingShader(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y)
    {
        // 物体がなければフォグカラー
        float depthSampled = gbuffers.depth.SampleColor(x, y).x();

        // 無限遠だったら
        if (depthSampled == numeric_limits<float>::max())
        {
            Vector3f fogColor = environment.fogColor;
            // スカイテクスチャが設定されてたらスカイボックス描画
            if (environment.skySphere.has_value())
            {
                Vector2f posNormed = Vector2f((float)x / gbuffers.screenSize.x() * 2 - 1, (float)y / gbuffers.screenSize.y() * 2 - 1);
                Matrix4f viewNormal = Transform::ResetScale(Transform::ResetPosition(environment.viewMat.inverse()));
                Vector3f normedView = (viewNormal * Vector4f(posNormed.x(), posNormed.y(), 1, 1)).head<3>().normalized();
                Vector2f skyUV = TextureMath::UVMod1(TextureMath::RectangularToPolarUV(normedView) + environment.skySphereOffset);
                return gbuffers.beauty.SampleColor(x, y) + environment.skySphere->SampleColor01BiLinear(skyUV.x(), skyUV.y());
            }
            else // スカイテクスチャがないのでフォグ描画
            {
                return gbuffers.beauty.SampleColor(x, y) + fogColor;
            }
        }
        else
        {
            return gbuffers.beauty.SampleColor(x, y);
        }
    }
}