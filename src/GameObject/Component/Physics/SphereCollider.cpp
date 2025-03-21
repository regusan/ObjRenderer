#include "SphereCollider.hpp"
#include "BoxCollider.hpp"

namespace REngine::Component
{
    SphereCollider::SphereCollider()
    {
    }
    SphereCollider::SphereCollider(const float radius) : radius(radius)
    {
    }
    SphereCollider::SphereCollider(json args)
    {
    }
    SphereCollider::~SphereCollider()
    {
    }
    Hit SphereCollider::DetectHit(const weak_ptr<Collider> &other)
    {
        auto sharedOther = other.lock();
        auto sharedOwner = this->owner.lock();
        if (!sharedOther && !sharedOwner)
            return Hit();
        auto sharedOtherOwner = sharedOther->owner.lock();
        if (!sharedOtherOwner)
            return Hit();

        Hit retval;
        retval.thisCollider = dynamic_pointer_cast<SphereCollider>(shared_from_this());
        /// スフィア同士の衝突の場合
        if (auto sharedOtherSphere = dynamic_pointer_cast<SphereCollider>(sharedOther))
        {
            float dist = (sharedOtherOwner->GetWorldPosition() - sharedOwner->GetWorldPosition()).norm();

            retval.isOverlapp = retval.isBlockingHit = dist < this->radius + sharedOtherSphere->radius;
            if (retval.isOverlapp)
            {
                retval.hitCollider = other;
                retval.impactNormal = (sharedOwner->GetWorldPosition() - sharedOtherOwner->GetWorldPosition()).normalized();
                // 球同士の中心を結んだ線上で球同士がオーバーラップしている線分の長さ
                float overlappLineLength = this->radius + sharedOtherSphere->radius - dist;
                // 片方の球の中心から衝突地点の中心までの距離
                float thisCentorToOverlappCentorLength = this->radius - overlappLineLength / 2;
                retval.impactPoint = sharedOwner->GetWorldPosition() + -retval.impactNormal * thisCentorToOverlappCentorLength;
            }
        }
        else if (auto sharedOtherBox = dynamic_pointer_cast<BoxCollider>(sharedOther))
        {
            BoundingBox3D box(sharedOtherBox->size);
            auto invModelMat = sharedOtherOwner->getWorldMat().inverse();
            float sphereRadius = this->radius;
            Vector3f tmp = sharedOwner->GetWorldPosition();
            Vector3f fixedSphereCentor = (invModelMat * Vector4f(tmp.x(), tmp.y(), tmp.z(), 1)).head<3>();

            retval.isOverlapp = retval.isBlockingHit = box.IsOverlapp(fixedSphereCentor, sphereRadius);
            if (retval.isOverlapp)
            {
                retval.hitCollider = other;
                retval.impactNormal = box.ComputeNearestBoxFaceNormal(fixedSphereCentor);
                retval.impactNormal = (sharedOtherOwner->getWorldMat() * Vector4f(retval.impactNormal.x(), retval.impactNormal.y(), retval.impactNormal.z(), 1)).head<3>();
                retval.penetrationDepth = fmax(0, sphereRadius - (box.ComputeNearestBoxFacePos(fixedSphereCentor) - fixedSphereCentor).norm());
                retval.penetrationDepth = fmax(0, box.ComputePenetrationDist(fixedSphereCentor, sphereRadius));
                retval.isOverlapp = retval.penetrationDepth > sphereRadius * 0.1;
                retval.impactPoint = (sharedOwner->GetWorldPosition() + retval.impactNormal) * 0.5;
            }
        }
        return retval;
    }
    void SphereCollider::CaluculateMass()
    {
        // pi*r^2で体積を求め体積を更新
        if (this->isAutoCaluculateMassFromVolume)
            this->mass = this->radius * this->radius * M_PI * this->dencity;
    }

}
