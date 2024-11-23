#pragma once
#include "../header/ShaderHeader.hpp"
#include "../header/EigenHeader.hpp"
#include "../TransformMat.hpp"
#include <random>
using namespace std;
using namespace Transform;

/// @brief 頂点シェーダー
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
    out.positionNDC = Vector4f((out.positionCS.x() + 1) * 0.5 * in.screenSize.x(),
                               (out.positionCS.y() + 1) * 0.5 * in.screenSize.y(),
                               out.positionCS.z(), 1);
    // 法線変換
    out.normalOS = in.normal;                                                        // モデル座標
    out.normalWS = Transform::ResetPosition(ResetScale(in.modelMat)) * out.normalOS; // モデル座標→ワールド座標
    out.normalOS = Transform::ResetPosition(ResetScale(in.viewMat)) * out.normalWS;  // ワールド座標→カメラ座標
    return out;
}

/// @brief ピクセルシェーダー
/// @param in 入力データ格納構造体
/// @return 出力データ格納構造体
inline const PixcelOutputStandard PixcelStandard(const PixcelInputStandard &in)
{
    PixcelOutputStandard out;
    // float light = in.normalVS.head<3>().dot(Vector3f(1, -1, -1).normalized());
    out.color = in.normalVS.head<3>() * 255;
    return out;
}