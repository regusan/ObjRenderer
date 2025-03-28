#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "../header/EigenHeader.hpp"
#include "../header/ShaderHeader.hpp"
#include "../Models/Model.hpp"
#include "../header/MathHeader.hpp"

#include "RenderTarget.hpp"
using namespace std;
namespace RenderingPipeline
{
    inline bool isAllVertInFrustum(const vector<VertOutputStandard> &poly)
    {
        for (const VertOutputStandard &vert : poly)
        {
            bool inBox = true;
            inBox &= -1 < vert.positionNDC.x() && vert.positionNDC.x() < 1;
            inBox &= -1 < vert.positionNDC.y() && vert.positionNDC.y() < 1;
            inBox &= 0 < vert.positionNDC.z() && vert.positionNDC.z() < 1;
            if (!inBox) // フラスタム外の頂点が含まれていたらFalse
                return false;
        }
        return true; // 全ての頂点はフラスタム内
    }
    inline bool isAnyVertInFrustum(const vector<VertOutputStandard> &poly)
    {
        for (const VertOutputStandard &vert : poly)
        {
            bool inBox = true;
            inBox &= -1 < vert.positionNDC.x() && vert.positionNDC.x() < 1;
            inBox &= -1 < vert.positionNDC.y() && vert.positionNDC.y() < 1;
            inBox &= 0 < vert.positionNDC.z() && vert.positionNDC.z() < 1;
            if (inBox) // フラスタム内の頂点が含まれていたらTrue
                return true;
        }
        return false; // 全ての頂点はフラスタム外
    }

    /// @brief VertOut配列をPixcelIn配列に変換
    /// @param outs VertOut
    /// @return PixcelIn
    vector<PixcelInputStandard> VertOuts2PixcelIns(vector<VertOutputStandard> outs);

}