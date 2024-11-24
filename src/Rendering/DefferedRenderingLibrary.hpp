#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "../header/EigenHeader.hpp"
#include "../header/ShaderHeader.hpp"
#include "../Model.hpp"
#include "../TransformMat.hpp"
#include "GBuffers.hpp"
#include "RenderTarget.hpp"
using namespace std;
namespace RenderingPipeline
{
    namespace Deffered
    {
        /// @brief 3Dモデルをレンダーターゲットに描画する
        /// @param model 描画する3Dモデル
        /// @param in カメラやモデル座標情報
        /// @param rt 描画出力先
        void DefferedDrawModelWireframe(
            const Model &model,
            const VertInputStandard &in,
            GBuffers &gb,
            const VertOutputStandard (*vert)(const VertInputStandard &in),
            const PixcelOutputStandard (*pixcel)(const PixcelInputStandard &in));
        void SimpleForwardFillPolygon(
            const vector<PixcelInputStandard> &points,
            GBuffers &gb,
            const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in));
    }
}