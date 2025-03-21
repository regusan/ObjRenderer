#pragma once

#include "../header/EigenHeader.hpp"
namespace REngine
{
    class BoundingBox3D
    {
    private:
    public:
        ~BoundingBox3D() {}
        BoundingBox3D(const Vector3f &size) : size(size) {}

        Vector3f position = Vector3f::Zero();
        Vector3f size = Vector3f::Ones();
        bool IsPointInBox(const Vector3f &point)
        {
            Vector3f halfSize = this->size / 2;
            // 各軸について指定範囲内にいたら1
            return (-halfSize.x() <= point.x() && point.x() <= halfSize.x()) &&
                   (-halfSize.y() <= point.y() && point.y() <= halfSize.y()) &&
                   (-halfSize.z() <= point.z() && point.z() <= halfSize.z());
        }
        // TODO::ボックスのエッジでの計算が曖昧
        bool IsOverlapp(const Vector3f &center, const float radius)
        {
            Vector3f halfSize = this->size / 2 + Vector3f::Ones() * radius;
            // 各軸について指定範囲内にいたら1
            return (-halfSize.x() <= center.x() && center.x() <= halfSize.x()) &&
                   (-halfSize.y() <= center.y() && center.y() <= halfSize.y()) &&
                   (-halfSize.z() <= center.z() && center.z() <= halfSize.z());
        }
        float ComputePenetrationDist(const Vector3f &center, const float radius)
        {
            float distToFace = (this->ComputeNearestBoxFacePos(center) - center).norm();
            if (this->IsPointInBox(center))
                return radius + distToFace;
            else
                return radius - distToFace;
        }
        Vector3f ComputeNearestBoxFaceNormal(const Vector3f &point)
        {
            Vector3f normalizedPoint = point.array() / this->size.array();

            int axis;
            normalizedPoint.maxCoeff(&axis);

            Vector3f normal = Vector3f::Zero();

            // 最も大きい軸の方向に法線をセット（正負は `point` の位置に依存）
            normal[axis] = (point[axis] > 0) ? 1.0f : -1.0f;

            return normal;
        }
        Vector3f ComputeNearestBoxFacePos(const Vector3f &point)
        {
            Vector3f halfSize = this->size / 2;
            return Vector3f(
                fmin(halfSize.x(), fmax(-halfSize.x(), point.x())),
                fmin(halfSize.y(), fmax(-halfSize.y(), point.y())),
                fmin(halfSize.z(), fmax(-halfSize.z(), point.z())));
        }
    };
}
