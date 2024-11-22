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
        this->positionWS = vo.positionWS;
        this->positionCS = vo.positionCS;
        this->positionMS = vo.positionMS;
        this->positionDS = vo.positionDS;
        this->normalWS = vo.normalWS;
        this->normalCS = vo.normalCS;
        this->normalMS = vo.normalMS;
        this->faceColor = vo.faceColor;
        this->uv = vo.uv;
    }
    ~PixcelInputStandard() {}
    Vector4f positionWS = Vector4f(0, 0, 0, 1);
    Vector4f positionCS = Vector4f(0, 0, 0, 1);
    Vector4f positionMS = Vector4f(0, 0, 0, 1);
    Vector4f positionDS = Vector4f(0, 0, 0, 1);
    Vector4f normalWS = Vector4f(1, 0, 0, 1);
    Vector4f normalCS = Vector4f(1, 0, 0, 1);
    Vector4f normalMS = Vector4f(1, 0, 0, 1);
    Vector3f faceColor = Vector3f(255, 0, 255);
    Vector2f uv = Vector2f(0, 0);
};
