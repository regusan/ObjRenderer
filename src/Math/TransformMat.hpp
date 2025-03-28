#pragma once
#include "../header/EigenHeader.hpp"

/// @brief 行列系名前空間
namespace Transform
{
    Matrix4f MakeRotMatX(const float &degree);
    Matrix4f MakeRotMatY(const float &degree);
    Matrix4f MakeRotMatZ(const float &degree);
    Matrix4f MakeRotMat(const Vector3f &rotaiton);
    Matrix4f MakeMatOffset(const Vector3f &offset);
    Matrix4f MakeMatScale(const Vector3f &scale);
    Vector3f GetPositionFromMat(const Matrix4f &mat);
    Matrix4f ResetScale(const Matrix4f &mat);
    Matrix4f SetPosition(const Matrix4f &mat, const Vector4f &pos);
    Matrix4f ResetPosition(const Matrix4f &mat);
    Vector3f GetForwardVector(const Matrix4f &mat);
    Vector3f GetRightVector(const Matrix4f &mat);
    Vector3f GetUpVector(const Matrix4f &mat);

};

inline Matrix4f CalcHomographyMatrix(Vector4f a0, Vector4f a1, Vector4f a2, Vector4f a3, Vector4f b0, Vector4f b1, Vector4f b2, Vector4f b3)
{
    Matrix4f original = Matrix4f::Zero();
    original << a0.x(), a0.y(), a0.z(), a0.w(),
        a1.x(), a1.y(), a1.z(), a1.w(),
        a2.x(), a2.y(), a2.z(), a2.w(),
        a3.x(), a3.y(), a3.z(), a3.w();
    Matrix4f transformed = Matrix4f::Zero();
    transformed << b0.x(), b0.y(), b0.z(), b0.w(),
        b1.x(), b1.y(), b1.z(), b1.w(),
        b2.x(), b2.y(), b2.z(), b2.w(),
        b3.x(), b3.y(), b3.z(), b3.w();
    return original.inverse() * transformed;
}

/// @brief 指定点の重心座標系での補完率を出力
/// @param a 頂点
/// @param b 頂点
/// @param c 頂点
/// @param P サンプル地点
/// @return 補完率(uvw)
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