#pragma once
/*
3D系の数学ライブラリ
*/

#include "../header/EigenHeader.hpp"
using namespace std;

/// @brief 0~1に丸める
/// @param x
/// @return 0~1
inline float saturate(const float x)
{
    return (x > 1.0f)   ? 1.0f
           : (x < 0.0f) ? 0.0f
                        : x;
}
inline Matrix3f CalculateTBN(const Vector3f &normal)
{
    // 適切な基準ベクトルを選択
    Vector3f reference = (std::abs(normal.z()) < 0.9f) ? Vector3f(0, 0, 1) : Vector3f(0, 1, 0);

    // Tangent（接線ベクトル）を計算
    Vector3f tangent = reference.cross(normal).normalized();

    // Bitangent（従法線ベクトル）を計算
    Vector3f bitangent = normal.cross(tangent).normalized();

    Matrix3f TBN;
    TBN.col(0) = tangent.normalized();
    TBN.col(1) = bitangent.normalized();
    TBN.col(2) = normal.normalized();
    return TBN;
}

class BoundingBox
{
public:
    int left;
    int right;
    int up;
    int bottom;

    // 通常のコンストラクタ
    BoundingBox(int left, int right, int up, int bottom)
        : left(left), right(right), up(up), bottom(bottom) {}

    // 円から矩形を作るコンストラクタ
    BoundingBox(Vector2f center, float radius)
        : left(center.x() - radius), right(center.x() + radius),
          up(center.y() - radius), bottom(center.y() + radius) {}

    // 交差判定（オーバーロード）
    bool operator&(const BoundingBox &other) const
    {
        return !(right < other.left || // 右端が相手の左端より左
                 left > other.right || // 左端が相手の右端より右
                 bottom < other.up ||  // 下端が相手の上端より上
                 up > other.bottom);   // 上端が相手の下端より下
    }

    friend std::ostream &operator<<(std::ostream &os, const BoundingBox &box)
    {
        os << "BoundingBox(left=" << box.left
           << ", right=" << box.right
           << ", up=" << box.up
           << ", bottom=" << box.bottom << ")";
        return os;
    }
};

namespace MathPhysics
{
    inline Vector3f Reflect(const Vector3f &in, const Vector3f &normal)
    {
        return (in - 2.0f * in.dot(normal) * normal).normalized();
    }

    inline Vector3f ComputePostCollisionVelocity(Vector3f v1, Vector3f v2, Vector3f normal, float m1 = 1.0f, float m2 = 1.0f, float e = 0.7f)
    {
        // 法線ベクトルの正規化
        Vector3f n = normal.normalized();

        // 各物体の法線方向の速度成分を計算
        float v1n = v1.dot(n);
        float v2n = v2.dot(n);

        // 接線方向の速度成分（変化しない）を計算
        Vector3f v1t = v1 - v1n * n;

        // 運動量保存と反発係数に基づく衝突後の法線方向速度
        float v1n_after = (m1 * v1n + m2 * v2n - m2 * e * (v1n - v2n)) / (m1 + m2);

        // 衝突が実際に起きている場合のみ速度を更新（速度が離れる方向の場合は衝突しない）
        if (v1n <= v2n)
        {
            // すでに離れる方向に動いている場合は速度を変更しない
            return v1;
        }

        // 法線方向と接線方向の速度を組み合わせて最終速度を返す
        return v1t + v1n_after * n;
    }
}
namespace StandardMath
{
    template <typename T>
    inline T lerp(const float ratio, const T &zero, const T &one)
    {
        return zero * (1 - ratio) + ratio * one;
    }

    inline float saturate(const float value)
    {
        return (value < 0) ? 0 : (value > 1) ? 1
                                             : value;
    }

    inline Vector3f clampv(const Vector3f &x, const float min, const float max)
    {
        return Vector3f(clamp<float>(x.x(), min, max), clamp<float>(x.y(), min, max), clamp<float>(x.z(), min, max));
    }
    inline Vector2f clampv(const Vector2f &x, const float min, const float max)
    {
        return Vector2f(clamp<float>(x.x(), min, max), clamp<float>(x.y(), min, max));
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
    /// @brief 三角形の求積
    /// @param v0
    /// @param v1
    /// @param v2
    /// @return
    inline float ComputeTriangleArea(const Vector2f &v0, const Vector2f &v1, const Vector2f &v2)
    {
        Vector2f ab = v1 - v0;
        Vector2f ac = v2 - v0;

        return 0.5f * fabs((ab.x() * ac.y() - ab.y() * ac.x()));
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

namespace TextureMath
{
    inline Vector2f UVMod1(const Vector2f &uv)
    {
        return Vector2f(fmod(uv.x(), 1.0f), fmod(uv.y(), 1.0f));
    }

    inline Vector3f PolarUVToRectangular(const float radius, const Vector2f uvYawPitch01)
    {
        float pitch = ((uvYawPitch01.x() * 2) - 1) * M_PI_2; // 0~1→-pi/2~pi/2
        float yaw = ((uvYawPitch01.y() * 1)) * M_PI;         //-pi~pi
        // yaw: 方位角, pitch: 仰角
        float x = radius * sinf(pitch) * cosf(yaw);
        float y = radius * cosf(pitch);
        float z = radius * sinf(pitch) * sinf(yaw);
        return Vector3f(x, y, z);
    }
    inline Vector2f RectangularToPolarUV(const Vector3f &dirWS)
    {
        Vector3f normedDirWS = dirWS.normalized();
        float th = acosf(normedDirWS.y());
        float p = atan2(normedDirWS.z(), normedDirWS.x());
        float phi = (p < 0) ? (p + M_PI * 2) : p;
        return Vector2f(clamp<float>(phi * (M_1_PI * 0.5), 0.0f, 1.0f),
                        clamp<float>(th * M_1_PI, 0.0f, 1.0f));
    }
}