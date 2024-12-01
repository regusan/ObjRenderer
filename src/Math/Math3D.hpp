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