#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <omp.h>
#include "../header/EigenHeader.hpp"
#include "../header/ShaderHeader.hpp"
#include "../Models/Model.hpp"
#include "../TransformMat.hpp"
#include "GBuffers.hpp"
#include "RenderTarget.hpp"
#include "ForwardRenderingLibrary.hpp"
using namespace std;
namespace RenderingPipeline
{
    namespace Deffered
    {
        /// @brief DefferedRenderingで3Dモデルをレンダーターゲットに描画する
        /// @param model 描画する3Dモデル
        /// @param in カメラやモデル座標情報
        /// @param gb 描画出力先GBuffer
        /// @param vert 利用する頂点シェーダーポインタ
        /// @param pixcel 利用するピクセルシェーダーポインタ
        void ExecGeometryPass(
            Model &model,
            const VertInputStandard &in,
            GBuffers &gb,
            const VertOutputStandard (*vert)(const VertInputStandard &in),
            const PixcelOutputStandard (*pixcel)(const PixcelInputStandard &in));

        /// @brief DefferedRenderingで指定したポリゴンをFillして描画
        /// @param points ポリゴンを構成する点群
        /// @param gb 描画先GBuffer
        /// @param pixcel 利用するピクセルシェーダーポインタ
        void SimpleDefferedFillPolygon(
            const vector<PixcelInputStandard> &points,
            GBuffers &gb,
            const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in));
    }
}