#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <omp.h>
#include "../header/EigenHeader.hpp"
#include "../header/ShaderHeader.hpp"
#include "../Models/Model.hpp"
#include "../header/MathHeader.hpp"
#include "GBuffers.hpp"
#include "RenderTarget.hpp"
#include "CommonRenderingLibrary.hpp"
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
        /// @brief 3Dモデルをワイヤフレームでレンダーターゲットに描画する
        /// @param model 描画する3Dモデル
        /// @param in カメラやモデル座標情報
        /// @param gb 描画出力先

        /// @brief DefferedRenderingで指定したポリゴンをFillして描画
        /// @param points ポリゴンを構成する点群
        /// @param gb 描画先GBuffer
        /// @param pixcel 利用するピクセルシェーダーポインタ
        void SimpleDefferedFillPolygon(
            const vector<PixcelInputStandard> &points,
            GBuffers &gb,
            const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in));
    }
    namespace Lighting
    {
        void ExecLightGeometryPass(
            Model &model,
            const VertInputStandard &in,
            GBuffers &gb,
            const VertOutputStandard (*vert)(const VertInputStandard &in),
            const PixcelOutputStandard (*pixcel)(const PixcelInputStandard &in));
        void FillLightPolygon(
            const vector<PixcelInputStandard> &points,
            Vector3f centorPosVS,
            GBuffers &gb,
            const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in));
    }
    namespace Tessellation
    {
        /// @brief 三角形を指定面積に分割するのに必要な回数を計算
        /// @param positionSS0
        /// @param positionSS1
        /// @param positionSS2
        /// @param thresholdArea
        /// @param maxTessCount
        /// @return
        inline float CalcTessLevel(const Vector2f &positionSS0, const Vector2f &positionSS1, const Vector2f &positionSS2, float thresholdArea = 200, int maxTessCount = 5)
        {
            float area = GeometryMath::ComputeTriangleArea(positionSS0, positionSS1, positionSS2);
            // return clamp<float>(log(area / thresholdArea) / log(1.0f / 4.0f), 0, maxTessCount); //(1/3)^N = area / thresholdAreaを解く
            return clamp<float>(log2(area / thresholdArea), 0, maxTessCount); //(1/4)^N=area / thresholdAreaを解く
        }
        /// @brief 三角形を4分割する
        /// @param points
        /// @return
        vector<vector<VertInputStandard>> TessellateTriangle4(const vector<VertInputStandard> &points);

        /// @brief ポリゴンが指定の面積になるまで分割
        /// @param points
        /// @param tessellateCount
        /// @return
        vector<vector<VertInputStandard>> TessellateSpecificArea(const vector<VertInputStandard> &points, const int tessellateCount);
    }
    namespace Forward
    {
        void ExecWireFramePass(
            Model &model,
            const VertInputStandard &in,
            GBuffers &gb,
            const VertOutputStandard (*vert)(const VertInputStandard &in));
    }
}