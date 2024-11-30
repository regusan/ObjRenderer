#pragma once

#include "../header/EigenHeader.hpp"
#include "VertOutputStandard.hpp"
class PixcelInputStandard
{
private:
    /* data */
public:
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
    PixcelInputStandard(/* args */) {}

    PixcelInputStandard(const VertOutputStandard &vo)
    {
        this->positionOS = vo.positionOS;
        this->positionWS = vo.positionWS;
        this->positionVS = vo.positionVS;
        this->positionCS = vo.positionCS;
        this->positionNDC = vo.positionNDC;
        this->positionSS = vo.positionSS;

        this->normalOS = vo.normalOS;
        this->normalWS = vo.normalWS;
        this->normalVS = vo.normalVS;

        this->vertColor = vo.vertColor;
        this->uv = vo.uv;

        this->material = vo.material;
    }
    ~PixcelInputStandard() {}

    static inline PixcelInputStandard lerp(const PixcelInputStandard &a, const PixcelInputStandard &b, const float &r)
    {
        PixcelInputStandard lerped;
        lerped.positionOS = a.positionOS * r + b.positionOS * (1 - r);
        lerped.positionWS = a.positionWS * r + b.positionWS * (1 - r);
        lerped.positionVS = a.positionVS * r + b.positionVS * (1 - r);
        lerped.positionCS = a.positionCS * r + b.positionCS * (1 - r);
        lerped.positionNDC = a.positionNDC * r + b.positionNDC * (1 - r);
        lerped.positionSS = a.positionSS * r + b.positionSS * (1 - r);

        lerped.normalOS = a.normalOS * r + b.normalOS * (1 - r);
        lerped.normalWS = a.normalWS * r + b.normalWS * (1 - r);
        lerped.normalVS = a.normalVS * r + b.normalVS * (1 - r);

        lerped.vertColor = a.vertColor * r + b.vertColor * (1 - r);
        lerped.uv = a.uv * r + b.uv * (1 - r);
        return lerped;
    }
    // バリセントリック座標
    static inline PixcelInputStandard barycentricLerp(
        const PixcelInputStandard &a, const PixcelInputStandard &b, const PixcelInputStandard &c,
        const float &u, const float &v, const float &w)
    {
        PixcelInputStandard lerped;

        lerped.positionOS = a.positionOS * u + b.positionOS * v + c.positionOS * w;
        lerped.positionWS = a.positionWS * u + b.positionWS * v + c.positionWS * w;
        lerped.positionVS = a.positionVS * u + b.positionVS * v + c.positionVS * w;
        lerped.positionCS = a.positionCS * u + b.positionCS * v + c.positionCS * w;
        lerped.positionNDC = a.positionNDC * u + b.positionNDC * v + c.positionNDC * w;
        lerped.positionSS = a.positionSS * u + b.positionSS * v + c.positionSS * w;

        lerped.normalOS = a.normalOS * u + b.normalOS * v + c.normalOS * w;
        lerped.normalWS = a.normalWS * u + b.normalWS * v + c.normalWS * w;
        lerped.normalVS = a.normalVS * u + b.normalVS * v + c.normalVS * w;

        lerped.vertColor = a.vertColor * u + b.vertColor * v + c.vertColor * w;
        lerped.uv = a.uv * u + b.uv * v + c.uv * w;

        lerped.material = a.material;

        return lerped;
    }
};
