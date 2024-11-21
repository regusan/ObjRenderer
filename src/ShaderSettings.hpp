#pragma once

#include "header/EigenHeader.hpp"

struct VertInputStandard
{
    Vector4f position = Vector4f(0, 0, 0, 1);
    Vector4f normal = Vector4f(1, 0, 0, 1);
    Matrix4f modelMat = Matrix4f::Identity();
    Matrix4f viewMat = Matrix4f::Identity();
    Vector3f vertColor = Vector3f(255, 0, 255);
};

struct VertOutputStandard
{
    Vector4f positionDS;
    Vector3f faceColor;
    /*
        inline PixcelInputStandard toPixcelInput()
        {
            PixcelInputStandard pi;
            pi.positionDS = this->positionDS;
            return pi;
        };
        */
};

struct PixcelInputStandard
{
    Vector4f positionWS = Vector4f(0, 0, 0, 1);
    Vector4f positionCS = Vector4f(0, 0, 0, 1);
    Vector4f positionMS = Vector4f(0, 0, 0, 1);
    Vector2f positionDS = Vector2f(0, 0);
    Vector4f normalWS = Vector4f(1, 0, 0, 1);
    Vector4f normalCS = Vector4f(1, 0, 0, 1);
    Vector4f normalMS = Vector4f(1, 0, 0, 1);
    Vector3f faceColor = Vector3f(255, 0, 255);
    Vector2f uv = Vector2f(0, 0);
};

struct PixcelOutputStandard
{
    Vector3f color;
};
