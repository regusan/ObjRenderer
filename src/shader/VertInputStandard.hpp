#pragma once

#include "../header/EigenHeader.hpp"

class VertInputStandard
{
private:
    /* data */
public:
    VertInputStandard(/* args */) {}
    ~VertInputStandard() {}
    Vector4f position = Vector4f(0, 0, 0, 1);
    Vector4f normal = Vector4f(1, 0, 0, 1);
    Matrix4f modelMat = Matrix4f::Identity();
    Matrix4f viewMat = Matrix4f::Identity();
    Vector4f vertColor = Vector4f(255, 0, 255, 255);
    Vector2i screenSize = Vector2i(1, 0);
};