#pragma once
#include "../header/ShaderHeader.hpp"
#include "../header/EigenHeader.hpp"
using namespace std;

/// @brief 頂点シェーダー
/// @param in VertOutputStandard
/// @return VertOutputStandard
inline const VertOutputStandard VertStandard(const VertInputStandard &in)
{
    VertOutputStandard out;
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
                              out.positionCS.z(),
                              1);
    out.normalMS = in.normal;
    out.normalWS = in.modelMat * in.normal;
    out.normalPS = in.viewMat * out.normalWS;
    return out;
}

inline const PixcelOutputStandard PixcelStandard(const PixcelInputStandard &in)
{
    PixcelOutputStandard out;
    out.color = Vector3f(in.normalMS.head<3>() * 255);
    return out;
}