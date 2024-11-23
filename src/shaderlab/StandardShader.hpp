#pragma once
#include "../header/ShaderHeader.hpp"
#include "../header/EigenHeader.hpp"
using namespace std;

/// @brief 頂点シェーダー
/// @param in 入力データ格納構造体
/// @return 出力データ格納構造体
inline const VertOutputStandard VertStandard(const VertInputStandard &in)
{
    VertOutputStandard out;
    // 座標変換
    out.positionMS = in.position;                 // モデル座標
    out.positionWS = in.modelMat * in.position;   // モデル座標→ワールド座標
    out.positionPS = in.viewMat * out.positionWS; // ワールド座標→カメラ座標
    // カメラ座標→クリップ座標（Z割)
    out.positionCS = Vector4f(out.positionPS.x() / out.positionPS.z(),
                              out.positionPS.y() / out.positionPS.z(),
                              out.positionPS.z(), 1);
    // クリップ座標→正規化デバイス座標
    out.positionDS = Vector4f((out.positionCS.x() + 1) * 0.5 * in.screenSize.x(),
                              (out.positionCS.y() + 1) * 0.5 * in.screenSize.y(),
                              out.positionCS.z(), 1);
    // 法線変換
    out.normalMS = in.normal;                 // モデル座標
    out.normalWS = in.modelMat * in.normal;   // モデル座標→ワールド座標
    out.normalPS = in.viewMat * out.normalWS; // ワールド座標→カメラ座標
    return out;
}

/// @brief ピクセルシェーダー
/// @param in 入力データ格納構造体
/// @return 出力データ格納構造体
inline const PixcelOutputStandard PixcelStandard(const PixcelInputStandard &in)
{
    PixcelOutputStandard out;
    out.color = Vector3f(in.normalMS.head<3>() * 255);
    return out;
}