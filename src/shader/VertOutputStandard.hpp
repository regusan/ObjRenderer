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
    Vector4f positionDS;
    Vector3f faceColor;

    inline PixcelInputStandard toPixcelInput()
    {
        PixcelInputStandard pi;
        pi.positionDS = this->positionDS.head<2>();
        return pi;
    };
};
