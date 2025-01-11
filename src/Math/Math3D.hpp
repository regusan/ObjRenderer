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
namespace GeometryMath
{
    inline Vector3f ComputeFaceNormal(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2)
    {
        Vector3f ab = v1 - v0;
        Vector3f ac = v2 - v0;

        Vector3f normal = ab.cross(ac).normalized();

        return normal;
    }
    inline uint xorshift(uint y)
    {
        // shift して xor する
        y = y ^ (y << 13);
        y = y ^ (y >> 17);
        y = y ^ (y << 5);

        // [0, 1] に正規化して返す
        return y;
    }

    inline Vector3f MakePointOnSphere(const Vector3f &centor, const float radius, const float theta, const float phi)
    {
        float x = radius * sin(phi) * cos(theta) + centor.x();
        float y = radius * sin(phi) * sin(theta) + centor.y();
        float z = radius * cos(phi) + centor.z();
        return Vector3f(x, y, z);
    }
    inline Vector3f MakeRandomPointInSphereByUVSeed(const Vector3f &centor, const float maxRadius, uint seed)
    {
        seed = xorshift(seed);
        float theta = (float)seed * 0.0001;
        seed = xorshift(seed);
        float phi = (float)seed * 0.0001;
        seed = xorshift(seed);
        float radius = sqrt(fmod(seed * 0.0001, maxRadius));
        return MakePointOnSphere(centor, radius, theta, phi);
    }
    inline Vector3f MakeRandomPointInHalfSphereByUVSeed(const Vector3f &centor, const float maxRadius, uint seed)
    {
        seed = xorshift(seed);
        float theta = (float)seed * 0.0001;
        seed = xorshift(seed);
        float phi = (float)seed * 0.0001;
        seed = xorshift(seed);
        float radius = fmod(seed * 0.0001, maxRadius);

        float x = radius * sin(phi) * cos(theta) + centor.x();
        float y = radius * sin(phi) * sin(theta) + centor.y();
        float z = radius * cos(phi) + centor.z();
        z = abs(z);
        return Vector3f(x, y, z);
    }
    inline Vector3f RotateVectorToBasis(const Vector3f &v, const Vector3f &dir)
    {
        Vector3f zAxis(0, 0, 1);
        Vector3f axis = zAxis.cross(dir).normalized();   // 回転軸を計算
        float angle = acos(zAxis.dot(dir.normalized())); // 回転角を計算

        // ロドリゲスの回転公式による回転行列適用
        Vector3f rotatedA = v * cos(angle) + axis.cross(v) * sin(angle) + axis * (axis.dot(v)) * (1 - cos(angle));

        return rotatedA;
    }

}