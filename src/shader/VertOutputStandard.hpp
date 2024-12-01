#pragma once

#include "PixcelInputStandard.hpp"
#include "../Models/Material.hpp"

/// @brief 頂点シェーダーの出力データクラス
class VertOutputStandard
{
private:
    /* data */
public:
    VertOutputStandard(/* args */) {}
    ~VertOutputStandard() {}

    Vector4f positionOS = Vector4f(0, 0, 0, 1);  // オブジェクト基準位置
    Vector4f positionWS = Vector4f(0, 0, 0, 1);  // ワールド基準位置
    Vector4f positionVS = Vector4f(0, 0, 0, 1);  // ビュー基準位置
    Vector4f positionCS = Vector4f(0, 0, 0, 1);  // クリップ基準位置
    Vector4f positionNDC = Vector4f(0, 0, 0, 1); // デバイス基準位置
    Vector4f positionSS = Vector4f(0, 0, 0, 1);  // スクリーン基準位置

    Vector4f normalOS = Vector4f(1, 0, 0, 1); // オブジェクト基準法線
    Vector4f normalWS = Vector4f(1, 0, 0, 1); // ワールド基準法線
    Vector4f normalVS = Vector4f(1, 0, 0, 1); // ビュー基準法線

    Vector4f vertColor = Vector4f(1, 0, 1, 1); //  デバイス基準法線
    Vector2f uv = Vector2f(0, 0);
    Material *material = nullptr;
};
