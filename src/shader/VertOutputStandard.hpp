#pragma once

#include "PixcelInputStandard.hpp"

#include "../header/EigenHeader.hpp"

class VertOutputStandard
{
private:
    /* data */
public:
    VertOutputStandard(/* args */) {}
    ~VertOutputStandard() {}
    Vector4f positionWS = Vector4f(0, 0, 0, 1);
    Vector4f positionCS = Vector4f(0, 0, 0, 1);
    Vector4f positionMS = Vector4f(0, 0, 0, 1);
    Vector4f positionDS = Vector4f(0, 0, 0, 1);
    Vector4f normalWS = Vector4f(1, 0, 0, 1);
    Vector4f normalCS = Vector4f(1, 0, 0, 1);
    Vector4f normalMS = Vector4f(1, 0, 0, 1);
    Vector3f faceColor = Vector3f(255, 0, 255);
    Vector2f uv = Vector2f(0, 0);

    inline PixcelInputStandard toPixcelInput()
    {
        PixcelInputStandard pi;
        pi.positionDS = this->positionDS;
        return pi;
    };
};
