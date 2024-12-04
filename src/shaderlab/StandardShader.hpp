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
    VertOutputStandard out;
    // 座標変換
    out.positionOS = in.position;                  // モデル座標
    out.positionWS = in.modelMat * out.positionOS; // モデル座標→ワールド座標
    out.positionVS = in.viewMat * out.positionWS;  // ワールド座標→カメラ座標
    // カメラ座標→クリップ座標（Z割)
    out.positionCS = Vector4f(out.positionVS.x() / out.positionVS.z(),
                              out.positionVS.y() / out.positionVS.z(),
                              out.positionVS.z(), 1);
    // クリップ座標→正規化デバイス座標
    out.positionNDC = Vector4f(out.positionCS.x(), out.positionCS.y(), (out.positionCS.z() - in.nearClip) / (in.farClip - in.nearClip), 1);
    // 正規化デバイス座標系→ディスプレイ座標系
    out.positionSS = Vector4f((out.positionNDC.x() + 1) * 0.5 * in.screenSize.x(),
                              (out.positionNDC.y() + 1) * 0.5 * in.screenSize.y(),
                              out.positionNDC.z(), 1);
    // 法線変換
    out.normalOS = in.normal;                                                        // モデル座標
    out.normalWS = Transform::ResetPosition(ResetScale(in.modelMat)) * out.normalOS; // モデル座標→ワールド座標
    out.normalVS = Transform::ResetPosition(ResetScale(in.viewMat)) * out.normalWS;  // ワールド座標→カメラ座標

    out.uv = in.uv;
    out.material = in.material;
    return out;
}

/// @brief ピクセルシェーダーの例
/// @param in 入力データ格納構造体
/// @return 出力データ格納構造体
inline const PixcelOutputStandard PixcelStandard(const PixcelInputStandard &in)
{
    PixcelOutputStandard out;
    Vector3f light0 = Vector3f(1, -1, -1).normalized();
    float ambientLight = 0.1;

    Vector3f ref = MathPhysics::Reflect(light0, in.normalVS.head<3>());
    float refval = ref.dot(Vector3f(0, 0, -1));
    float specularScalar = abs(refval) * in.material->specularShapness / 1000.0f;
    Vector3f specular = in.material->specular * specularScalar;

    float light = in.normalWS.head<3>().dot(light0);
    light = clamp<float>(light, ambientLight, 1.0f);

    optional<RenderTarget> &diff = in.material->diffuseMap;
    if (diff)
        out.diffuse = diff->SampleColor(diff->getScreenSize().x() * in.uv.x(),
                                        diff->getScreenSize().y() * in.uv.y());
    else
        out.diffuse = in.material->diffuse;
    out.color = out.diffuse * light + specular;
    return out;
}