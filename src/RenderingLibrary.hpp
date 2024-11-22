#pragma once
#include "Model.hpp"
#include "RenderTarget.hpp"
#include <vector>

#include <iostream>
#include "header/EigenHeader.hpp"
#include "header/ShaderHeader.hpp"
using namespace std;

/// @brief 3Dモデルをレンダーターゲットに描画する
/// @param model 描画する3Dモデル
/// @param in カメラやモデル座標情報
/// @param rt 描画出力先
void DrawModelWireframe(const Model &model,
                        const VertInputStandard &in,
                        RenderTarget &rt,
                        const VertOutputStandard (*vert)(const VertInputStandard &in),
                        const PixcelOutputStandard (*pixcel)(const PixcelInputStandard &in));

void DrawLine(const PixcelInputStandard &start,
              const PixcelInputStandard &end,
              RenderTarget &rt,
              const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in));
void DrawPolygonLine(const vector<PixcelInputStandard> &points,
                     RenderTarget &rt,
                     const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in));