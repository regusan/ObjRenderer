#pragma once
#include "header/ShaderHeader.hpp"
#include "Model.hpp"
#include "RenderTarget.hpp"
#include "RenderingLibrary.hpp"

#include <iostream>
#include "header/EigenHeader.hpp"
using namespace std;

/// @brief 頂点シェーダー
/// @param in VertOutputStandard
/// @return VertOutputStandard
inline const VertOutputStandard vert(const VertInputStandard &in)
{
    VertOutputStandard out;
    // モデル座標→ワールド座標→カメラ座標
    out.positionDS = in.viewMat * in.modelMat * in.position;
    // カメラ座標→クリップ座標（Z割)
    out.positionDS = Vector4f(out.positionDS.x() / out.positionDS.z(),
                              out.positionDS.y() / out.positionDS.z(),
                              out.positionDS.z(), 1);
    out.normalWS = in.normal;
    return out;
}

inline const PixcelOutputStandard pixcel(const PixcelInputStandard &in)
{
    PixcelOutputStandard out;
    out.color = Vector3f(in.normalWS.head<3>() * 255);
    return out;
}