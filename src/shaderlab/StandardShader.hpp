#pragma once
#include "../header/ShaderHeader.hpp"
#include "../header/EigenHeader.hpp"
#include "../TransformMat.hpp"
#include "../Math/Math3D.hpp"
#include <random>
using namespace std;
using namespace Transform;

/// @brief 頂点シェーダーの例
/// @param in 入力データ格納構造体
/// @return 出力データ格納構造体
inline const VertOutputStandard VertStandard(const VertInputStandard &in)
{
    VertOutputStandard out(in.environment);

    // 座標変換
    out.positionOS = in.position;                  // モデル座標
    out.positionWS = in.modelMat * out.positionOS; // モデル座標→ワールド座標
    out.positionVS = in.viewMat * out.positionWS;  // ワールド座標→カメラ座標
    // カメラ座標→クリップ座標（Z割)
    out.positionCS = Vector4f(out.positionVS.x() / out.positionVS.z(),
                              out.positionVS.y() / out.positionVS.z(),
                              out.positionVS.z(), 1);
    // クリップ座標→正規化デバイス座標
    out.positionNDC = Vector4f(out.positionCS.x(), out.positionCS.y(), (out.positionCS.z() - in.environment.nearClip) / (in.environment.farClip - in.environment.nearClip), 1);
    // 正規化デバイス座標系→ディスプレイ座標系
    out.positionSS = Vector4f((out.positionNDC.x() + 1) * 0.5 * in.environment.screenSize.x(),
                              (out.positionNDC.y() + 1) * 0.5 * in.environment.screenSize.y(),
                              out.positionNDC.z(), 1);
    // 法線変換
    out.normalOS = in.normal;                                                        // モデル座標
    out.normalWS = Transform::ResetPosition(ResetScale(in.modelMat)) * out.normalOS; // モデル座標→ワールド座標
    out.normalVS = Transform::ResetPosition(ResetScale(in.viewMat)) * out.normalWS;  // ワールド座標→カメラ座標

    out.uv = in.uv;
    out.material = in.material;
    return out;
}
Matrix3f CalculateTBN(const Vector3f &normalWS)
{
    // 適切な基準ベクトルを選択
    Vector3f reference = (std::abs(normalWS.z()) < 0.9f) ? Vector3f(0, 0, 1) : Vector3f(0, 1, 0);

    // Tangent（接線ベクトル）を計算
    Vector3f tangentWS = reference.cross(normalWS).normalized();

    // Bitangent（従法線ベクトル）を計算
    Vector3f bitangentWS = normalWS.cross(tangentWS).normalized();

    Matrix3f TBN;
    TBN.col(0) = tangentWS.normalized();   // Tangent
    TBN.col(1) = bitangentWS.normalized(); // Bitangent
    TBN.col(2) = normalWS.normalized();    // Normal (from GBuffer or vertex)
    return TBN;
}
/// @brief ピクセルシェーダーの例
/// @param in 入力データ格納構造体
/// @return 出力データ格納構造体
inline const PixcelOutputStandard PixcelStandard(const PixcelInputStandard &in)
{
    PixcelOutputStandard out;
    constexpr float InvSqrt3 = 0.57735026919f; // 1/sqrt(3)
    constexpr float InvMaxShinness = 1.0f / 1000.0f;

    out.specular = in.material->specular; // * in.material->specularShapness;

    if (in.material->shaderModel == Material::ShaderModel::Phoneg)
        out.ORM = Vector3f(1, in.material->specularShapness * InvMaxShinness, in.material->specular.norm() * InvSqrt3);
    if (in.material->shaderModel == Material::ShaderModel::PBR)
        out.ORM = Vector3f(1, in.material->pbrRoughness, in.material->pbrMetalic);
    if (in.material->roughnessMap)
        out.ORM.y() = in.material->roughnessMap->SampleColor01(fmod(in.uv.x(), 1), fmod(in.uv.y(), 1)).x();
    if (in.material->metalicMap)
        out.ORM.z() = in.material->metalicMap->SampleColor01(fmod(in.uv.x(), 1), fmod(in.uv.y(), 1)).x();

    out.diffuse = in.material->diffuse; // Diffuseをマテリアルから設定
    // DiffuseMapが存在するなら、テクスチャのディフューズを掛け合わせる
    if (in.material->diffuseMap)
        out.diffuse = out.diffuse.array() * in.material->diffuseMap->SampleColor01(fmod(in.uv.x(), 1), fmod(in.uv.y(), 1)).array();

    out.normalWS = in.normalWS.head<3>();
    out.normalVS = in.normalVS.head<3>();
    if (in.material->normalMap) // NormalMapが存在するなら、サンプル
    {
        // 法線マップをサンプリング
        Vector3f sampledNormal = in.material->normalMap->SampleColor01(in.uv.x(), in.uv.y()) * 2.0f - Vector3f(1.0f, 1.0f, 1.0f);

        // TBN行列を構築

        Matrix3f TBN = CalculateTBN(in.normalWS.head<3>());
        out.normalWS = (TBN * sampledNormal).normalized();

        TBN = CalculateTBN(in.normalVS.head<3>());
        out.normalVS = (TBN * sampledNormal).normalized();
        out.normalVS = sampledNormal;
    }

    out.emission = in.material->emission;
    out.color = out.diffuse;

    return out;
}