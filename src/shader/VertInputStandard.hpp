#pragma once

#include "../header/EigenHeader.hpp"

class VertInputStandard
{
private:
    /* data */
public:
    VertInputStandard(/* args */) {}
    ~VertInputStandard() {}
    float nearClip = 1;
    float farClip = 100;
    Vector4f position = Vector4f(0, 0, 0, 1);
    Vector4f normal = Vector4f(1, 0, 0, 1);
    Vector2f uv = Vector2f(0, 0);
    Matrix4f modelMat = Matrix4f::Identity();
    Matrix4f viewMat = Matrix4f::Identity();
    Vector4f vertColor = Vector4f(1, 0, 1, 1);
    Vector2i screenSize = Vector2i(1, 0);
};