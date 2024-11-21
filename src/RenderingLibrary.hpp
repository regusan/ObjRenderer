#pragma once
#include "Model.hpp"
#include "RenderTarget.hpp"

#include <iostream>
#include "header/EigenHeader.hpp"
#include "header/ShaderHeader.hpp"
using namespace std;

inline Vector4f clip2Device(const Vector4f &clippos, const Vector2i &screenSize);
vector<Vector2f> VertOut2Positions(const vector<VertOutputStandard> &outs);

/// @brief 3Dモデルをレンダーターゲットに描画する
/// @param model 描画する3Dモデル
/// @param in カメラやモデル座標情報
/// @param rt 描画出力先
void DrawModel(const Model &model,
               const VertInputStandard &in,
               RenderTarget &rt,
               const VertOutputStandard (*vert)(const VertInputStandard &in),
               const PixcelOutputStandard (*pixcel)(const PixcelInputStandard &in));
