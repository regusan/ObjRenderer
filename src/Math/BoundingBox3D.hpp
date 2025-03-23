#pragma once

#include "../header/EigenHeader.hpp"
namespace REngine
{
    class BoundingBox3D
    {
    private:
    public:
        ~BoundingBox3D();
        BoundingBox3D(const Vector3f &size);

        Vector3f position = Vector3f::Zero();
        Vector3f size = Vector3f::Ones();

        /// @brief 点がボックス内にいるか
        /// @param point
        /// @return
        bool IsPointInBox(const Vector3f &point);

        /// @brief ボックスと球がオーバーラップしているか。TODO::ボックスのエッジでの計算が曖昧
        /// @param center
        /// @param radius
        /// @return
        bool IsOverlapp(const Vector3f &center, const float radius);

        /// @brief ボックスと球の貫通している距離を求める
        /// @param center
        /// @param radius
        /// @return 符号付距離。正でオーバーラップ、負で非オーバーラップ
        float ComputePenetrationDist(const Vector3f &center, const float radius);
        /// @brief 指定点に最も近い平面の法線を返す
        /// @param point
        /// @return
        Vector3f ComputeNearestBoxFaceNormal(const Vector3f &point);
        /// @brief 指定点に最も近い面上の点を返す
        /// @param point
        /// @return
        Vector3f ComputeNearestBoxFacePos(const Vector3f &point);
    };
}
