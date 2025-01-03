#pragma once
/*
3D系の数学ライブラリ
*/

#include "../header/EigenHeader.hpp"

namespace MathPhysics
{
    inline Vector3f Reflect(const Vector3f &in, const Vector3f &normal)
    {
        return (in - 2.0f * in.dot(normal) * normal).normalized();
    }
}

inline Vector3f ComputeFaceNormal(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2)
{
    Vector3f ab = v1 - v0;
    Vector3f ac = v2 - v0;

    Vector3f normal = ab.cross(ac).normalized();

    return normal;
}