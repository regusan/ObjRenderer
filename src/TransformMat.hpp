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

static inline Vector3f computeBarycentricCoordinates(
    const VectorXf &a, const VectorXf &b, const VectorXf &c, const VectorXf &P)
{
    // 三角形の辺のベクトルを計算
    VectorXf v0 = b - a;
    VectorXf v1 = c - a;
    VectorXf v2 = P - a;

    // 三角形の面積を計算
    float d00 = v0.dot(v0);
    float d01 = v0.dot(v1);
    float d11 = v1.dot(v1);
    float d20 = v2.dot(v0);
    float d21 = v2.dot(v1);

    // バリセントリック座標を計算
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return Vector3f(u, v, w);
}