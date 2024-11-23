#pragma once

#include "../header/EigenHeader.hpp"
#include "VertOutputStandard.hpp"
class PixcelInputStandard
{
private:
    /* data */
public:
    PixcelInputStandard(/* args */) {}

    PixcelInputStandard(const VertOutputStandard &vo)
    {
        this->positionOS = vo.positionOS;
        this->positionWS = vo.positionWS;
        this->positionVS = vo.positionVS;
        this->positionCS = vo.positionCS;
        this->positionNDC = vo.positionNDC;

        this->normalOS = vo.normalOS;
        this->normalWS = vo.normalWS;
        this->normalVS = vo.normalOS;

        this->vertColor = vo.vertColor;
        this->uv = vo.uv;
    }
    ~PixcelInputStandard() {}
    Vector4f positionOS = Vector4f(0, 0, 0, 1);  // オブジェクト基準位置
    Vector4f positionWS = Vector4f(0, 0, 0, 1);  // ワールド基準位置
    Vector4f positionVS = Vector4f(0, 0, 0, 1);  // ビュー基準位置
    Vector4f positionCS = Vector4f(0, 0, 0, 1);  // クリップ基準位置
    Vector4f positionNDC = Vector4f(0, 0, 0, 1); // デバイス基準位置

    Vector4f normalOS = Vector4f(1, 0, 0, 1); // オブジェクト基準法線
    Vector4f normalWS = Vector4f(1, 0, 0, 1); // ワールド基準法線
    Vector4f normalVS = Vector4f(1, 0, 0, 1); // ビュー基準法線

    Vector4f vertColor = Vector4f(255, 0, 255, 255); //  デバイス基準法線
    Vector2f uv = Vector2f(0, 0);
};
