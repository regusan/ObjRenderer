#pragma once

#include "PixcelInputStandard.hpp"

// #include "../header/EigenHeader.hpp"

class VertOutputStandard
{
private:
    /* data */
public:
    VertOutputStandard(/* args */) {}
    ~VertOutputStandard() {}

    Vector4f positionMS = Vector4f(0, 0, 0, 1); //  モデル基準位置
    Vector4f positionWS = Vector4f(0, 0, 0, 1); // ワールド基準位置
    Vector4f positionPS = Vector4f(0, 0, 0, 1); //  カメラ基準位置
    Vector4f positionCS = Vector4f(0, 0, 0, 1); // クリップ基準位置
    Vector4f positionDS = Vector4f(0, 0, 0, 1); //  デバイス基準位置

    Vector4f normalMS = Vector4f(1, 0, 0, 1); //  モデル基準法線
    Vector4f normalWS = Vector4f(1, 0, 0, 1); // ワールド基準法線
    Vector4f normalPS = Vector4f(1, 0, 0, 1); // カメラ基準法線

    Vector4f vertColor = Vector4f(255, 0, 255, 255); //  デバイス基準法線
    Vector2f uv = Vector2f(0, 0);
};
