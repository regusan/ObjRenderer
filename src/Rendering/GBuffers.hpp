#pragma once

#include "../header/EigenHeader.hpp"
#include "../header/ShaderHeader.hpp"
#include "RenderTarget.hpp"
#include <limits>
#include <map>
using namespace std;

/// @brief DefferedRenderingのためのRenderTargetを格納したGBuffer
class GBuffers
{
private:
    /// @brief RenderTargetの名前とオブジェクトのマップ
    map<string, RenderTarget *> str2rt;

public:
    Vector2i screenSize;

    /*RenderTarget*/
    RenderTarget forward;
    RenderTarget beauty;

    RenderTarget diffuse;
    RenderTarget specular;
    RenderTarget emission;

    RenderTarget depth;
    RenderTarget AO;
    RenderTarget reflection;
    RenderTarget SSShadow;

    RenderTarget positionWS;
    RenderTarget positionVS;

    RenderTarget normalWS;
    RenderTarget normalVS;

    RenderTarget uv;
    RenderTarget temp;

    GBuffers(const int &width, const int &height);
    ~GBuffers();

    /// @brief RenderTargetをＰＰＭ画像として出力
    /// @param filepath 出力パス
    /// @param positionModValue 指定値でピクセル値をＭｏｄする
    /// @param normalMulValue 指定値でピクセル値を乗算する
    void writeAsPNG(const string &filepath,
                    const float positionModValue = 1,
                    const float normalMulValue = 1);
    /// @brief 名前からRenderTargetを取得
    /// @param rtname
    /// @return RenderTarget
    RenderTarget &getRTFromString(string rtname);
};
