#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "../header/EigenHeader.hpp"
#include "../header/ShaderHeader.hpp"
#include "../Models/Model.hpp"
#include "../TransformMat.hpp"

#include "RenderTarget.hpp"
using namespace std;
namespace RenderingPipeline
{
    inline bool isInFrustum(const vector<VertOutputStandard> &poly)
    {
        bool anyInBox = false;
        for (const VertOutputStandard &vert : poly)
        {
            bool inBox = true;
            inBox &= -1 < vert.positionNDC.x() && vert.positionNDC.x() < 1;
            inBox &= -1 < vert.positionNDC.y() && vert.positionNDC.y() < 1;
            inBox &= 0 < vert.positionNDC.z() && vert.positionNDC.z() < 1;
            anyInBox |= inBox;
            // if (!inBox) // フラスタム外の頂点が含まれていたらFalse
            //   return true;
        }
        return anyInBox; // 全ての頂点はフラスタム内
    }

    /// @brief VertOut配列をPixcelIn配列に変換
    /// @param outs VertOut
    /// @return PixcelIn
    vector<PixcelInputStandard> VertOuts2PixcelIns(vector<VertOutputStandard> outs);

}