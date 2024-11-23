#pragma once
#include "header/ShaderHeader.hpp"
#include "header/EigenHeader.hpp"

namespace Transform
{
    Matrix4f MakeRotMatX(const float &degree);
    Matrix4f MakeRotMatY(const float &degree);
    Matrix4f MakeRotMatZ(const float &degree);
    Matrix4f MakeRotMat(const Vector3f &rotaiton);
    Matrix4f MakeMatOffset(const Vector3f &offset);
    Vector4f GetPositionFromMat(const Matrix4f &mat);
    Matrix4f ResetScale(const Matrix4f &mat);
    Matrix4f SetPosition(const Matrix4f &mat, const Vector4f &pos);
    Matrix4f ResetPosition(const Matrix4f &mat);
};
