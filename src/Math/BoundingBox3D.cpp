#include "BoundingBox3D.hpp"
namespace REngine
{
    BoundingBox3D::~BoundingBox3D() {}
    BoundingBox3D::BoundingBox3D(const Vector3f &size) : size(size) {}

    Vector3f position = Vector3f::Zero();
    Vector3f size = Vector3f::Ones();
    bool BoundingBox3D::IsPointInBox(const Vector3f &point)
    {
        Vector3f halfSize = this->size / 2;
        Vector3f fixedPos = point - this->position;
        // 各軸について指定範囲内にいたら1
        return (-halfSize.x() <= fixedPos.x() && fixedPos.x() <= halfSize.x()) &&
               (-halfSize.y() <= fixedPos.y() && fixedPos.y() <= halfSize.y()) &&
               (-halfSize.z() <= fixedPos.z() && fixedPos.z() <= halfSize.z());
    }
    // TODO::ボックスのエッジでの計算が曖昧
    bool BoundingBox3D::IsOverlapp(const Vector3f &center, const float radius)
    {
        Vector3f fixedCentor = center - this->position; // ボックス基準へ
        Vector3f halfSize = this->size / 2 + Vector3f::Ones() * radius;
        // 各軸について指定範囲内にいたら1
        return (-halfSize.x() <= fixedCentor.x() && fixedCentor.x() <= halfSize.x()) &&
               (-halfSize.y() <= fixedCentor.y() && fixedCentor.y() <= halfSize.y()) &&
               (-halfSize.z() <= fixedCentor.z() && fixedCentor.z() <= halfSize.z());
    }
    float BoundingBox3D::ComputePenetrationDist(const Vector3f &center, const float radius)
    {
        float distToFace = (this->ComputeNearestBoxFacePos(center) - center).norm();
        if (this->IsPointInBox(center))
            return radius + distToFace;
        else
            return radius - distToFace;
    }
    Vector3f BoundingBox3D::ComputeNearestBoxFaceNormal(const Vector3f &point)
    {
        // 1〜1に正規か
        Vector3f normalizedPoint = point.array() / (this->size.array() * 0.5f);

        // 各座標の絶対値を取得
        Vector3f absNormalizedPoint = normalizedPoint.array().abs();

        // 最も1に近い軸を見つける=最も近い面の軸
        int axis;
        absNormalizedPoint.maxCoeff(&axis);

        Vector3f normal = Vector3f::Zero();

        // 最も近い面の法線を設定
        normal[axis] = (normalizedPoint[axis] > 0) ? 1.0f : -1.0f;

        return normal;
    }
    Vector3f BoundingBox3D::ComputeNearestBoxFacePos(const Vector3f &point)
    {
        Vector3f halfSize = this->size * 0.5f;
        return Vector3f(
            fmin(halfSize.x(), fmax(-halfSize.x(), point.x())),
            fmin(halfSize.y(), fmax(-halfSize.y(), point.y())),
            fmin(halfSize.z(), fmax(-halfSize.z(), point.z())));
    }

    std::vector<Vector3f> BoundingBox3D::GetVertices() const
    {
        Vector3f halfSize = this->size * 0.5f;
        std::vector<Vector3f> vertices = {
            position + Vector3f(-halfSize.x(), -halfSize.y(), -halfSize.z()),
            position + Vector3f(halfSize.x(), -halfSize.y(), -halfSize.z()),
            position + Vector3f(halfSize.x(), halfSize.y(), -halfSize.z()),
            position + Vector3f(-halfSize.x(), halfSize.y(), -halfSize.z()),
            position + Vector3f(-halfSize.x(), -halfSize.y(), halfSize.z()),
            position + Vector3f(halfSize.x(), -halfSize.y(), halfSize.z()),
            position + Vector3f(halfSize.x(), halfSize.y(), halfSize.z()),
            position + Vector3f(-halfSize.x(), halfSize.y(), halfSize.z())};

        return vertices;
    }
}