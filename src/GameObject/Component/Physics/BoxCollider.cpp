#include "SphereCollider.hpp"
#include "BoxCollider.hpp"

namespace REngine::Component
{
    BoxCollider::BoxCollider()
    {
    }
    BoxCollider::BoxCollider(const Vector3f size) : size(size)
    {
    }
    BoxCollider::BoxCollider(json args)
    {
    }
    BoxCollider::~BoxCollider()
    {
    }
    Hit BoxCollider::DetectHit(const weak_ptr<Collider> &other)
    {
        auto sharedOther = other.lock();
        auto sharedOwner = this->owner.lock();
        if (!sharedOther || !sharedOwner)
            return Hit();
        auto sharedOtherOwner = sharedOther->owner.lock();
        if (!sharedOtherOwner)
            return Hit();
        // オブジェクトが有効か検証
        if (!this->GetActive() || !sharedOther->GetActive())
            return Hit();
        Hit retval;
        retval.thisCollider = dynamic_pointer_cast<BoxCollider>(shared_from_this());

        // 球との衝突判定
        if (auto sharedOtherSphere = dynamic_pointer_cast<SphereCollider>(sharedOther))
        {
            BoundingBox3D box(this->size);
            auto invModelMat = sharedOwner->getWorldMat().inverse();
            float sphereRadius = sharedOtherSphere->radius;
            Vector3f tmp = sharedOtherOwner->GetWorldPosition();
            Vector3f fixedSphereCenter = (invModelMat * Vector4f(tmp.x(), tmp.y(), tmp.z(), 1)).head<3>();

            retval.isOverlapp = retval.isBlockingHit = box.IsOverlapp(fixedSphereCenter, sphereRadius);
            retval.isBlockingHit &= this->IsHitWith(sharedOther);
            retval.isOverlapp &= this->IsOverlapWith(sharedOther);

            if (retval.isOverlapp || retval.isBlockingHit)
            {
                retval.hitCollider = other;
                Vector3f normalOS = box.ComputeNearestBoxFaceNormal(fixedSphereCenter);
                retval.impactNormal = (sharedOwner->getWorldMat() * Vector4f(normalOS.x(), normalOS.y(), normalOS.z(), 0)).head<3>().normalized();
                retval.penetrationDepth = fmax(0, box.ComputePenetrationDist(fixedSphereCenter, sphereRadius));
                retval.isOverlapp = retval.penetrationDepth > sphereRadius * 0.1;

                Vector3f boxNearestPointOS = box.ComputeNearestBoxFacePos(fixedSphereCenter);
                retval.impactPoint = (sharedOwner->getWorldMat() * Vector4f(boxNearestPointOS.x(), boxNearestPointOS.y(), boxNearestPointOS.z(), 1)).head<3>();
            }
        }
        // もう一つのボックスとの衝突判定
        else if (auto sharedOtherBox = dynamic_pointer_cast<BoxCollider>(sharedOther))
        {
            BoundingBox3D thisBox(this->size);
            BoundingBox3D otherBox(sharedOtherBox->size);

            auto invModelMat = sharedOwner->getWorldMat().inverse();
            Vector3f otherBoxCenter = (invModelMat * Vector4f(sharedOtherOwner->GetWorldPosition().x(),
                                                              sharedOtherOwner->GetWorldPosition().y(),
                                                              sharedOtherOwner->GetWorldPosition().z(),
                                                              1))
                                          .head<3>();

            retval.isOverlapp = retval.isBlockingHit = thisBox.IsOverlapp(otherBoxCenter, sharedOtherBox->size.maxCoeff());
            retval.isBlockingHit &= this->IsHitWith(sharedOther);
            retval.isOverlapp &= this->IsOverlapWith(sharedOther);

            if (retval.isOverlapp || retval.isBlockingHit)
            {
                retval.hitCollider = other;
                Vector3f normalOS = thisBox.ComputeNearestBoxFaceNormal(otherBoxCenter);
                retval.impactNormal = (sharedOwner->getWorldMat() * Vector4f(normalOS.x(), normalOS.y(), normalOS.z(), 0)).head<3>().normalized();
                retval.penetrationDepth = fmax(0, thisBox.ComputePenetrationDist(otherBoxCenter, sharedOtherBox->size.maxCoeff()));
                retval.isOverlapp = retval.penetrationDepth > 0.1;

                Vector3f boxNearestPointOS = thisBox.ComputeNearestBoxFacePos(otherBoxCenter);
                retval.impactPoint = (sharedOwner->getWorldMat() * Vector4f(boxNearestPointOS.x(), boxNearestPointOS.y(), boxNearestPointOS.z(), 1)).head<3>();
            }
        }

        return retval;
    }
    void BoxCollider::CaluculateMass()
    {
        if (this->isAutoCaluculateMassFromVolume)
            this->mass = this->size.x() * this->size.y() * this->size.z() * this->physicMaterial.density;
    }
    float BoxCollider::GetBoundingRadius() const
    {

        if (auto sharedOwner = this->owner.lock())
            return Vector3f(this->size.array() * sharedOwner->GetWorldScale().array()).norm();
        return this->size.norm();
    }
    void BoxCollider::DrawDebugShape()
    {
        Collider::DrawDebugShape();
        if (auto sharedOwner = this->owner.lock())
            DebugDrawSubSystem::getInstance().DrawDebugBox(
                sharedOwner->GetWorldPosition(),
                sharedOwner->GetWorldRotation(),
                sharedOwner->GetWorldScale().array() * this->size.array(),
                this->colliderDebugColor);
    }
}
