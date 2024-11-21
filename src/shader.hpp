#pragma once
#include "ShaderSettings.hpp"
#include "Model.hpp"
#include "RenderTarget.hpp"
#include "RenderingLibrary.hpp"

#include <iostream>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>
using Eigen::Matrix4f;
using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Vector4f;
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
    return out;
}

inline const PixcelOutputStandard pixcel(const PixcelInputStandard &in)
{
    PixcelOutputStandard out;
    out.color = Vector3f(255, 0, 255);
    return out;
}