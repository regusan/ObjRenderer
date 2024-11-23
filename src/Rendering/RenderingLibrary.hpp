#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "../header/EigenHeader.hpp"
#include "../header/ShaderHeader.hpp"
#include "../Model.hpp"
#include "../TransformMat.hpp"
#include "RenderTarget.hpp"
using namespace std;
namespace RenderingPipeline
{
    /// @brief 3Dモデルをレンダーターゲットに描画する
    /// @param model 描画する3Dモデル
    /// @param in カメラやモデル座標情報
    /// @param rt 描画出力先
    void SimpleForwardDrawModelWireframe(const Model &model,
                                         const VertInputStandard &in,
                                         RenderTarget &rt,
                                         const VertOutputStandard (*vert)(const VertInputStandard &in),
                                         const PixcelOutputStandard (*pixcel)(const PixcelInputStandard &in));

    void SimpleForwardDrawLine(const PixcelInputStandard &start,
                               const PixcelInputStandard &end,
                               RenderTarget &rt,
                               const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in));
    void SimpleForwardDrawPolygonLine(const vector<PixcelInputStandard> &points,
                                      RenderTarget &rt,
                                      const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in));

    void SimpleForwardFillPolygon(const vector<PixcelInputStandard> &points,
                                  RenderTarget &rt,
                                  const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in));
}