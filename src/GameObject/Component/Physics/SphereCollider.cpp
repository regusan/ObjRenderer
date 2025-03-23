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

        // オブジェクトが有効か検証
        if (!this->GetActive() || !sharedOther->GetActive())
            return Hit();
        // 有効なレイヤー同士か検証
        if (!(this->collisionLayer & sharedOther->hitLayerMask || this->collisionLayer & sharedOther->overlapLayerMask))
            return Hit();

        Hit retval;
        retval.thisCollider = dynamic_pointer_cast<Collider>(shared_from_this());
        /// スフィアとの衝突の場合
        if (auto sharedOtherSphere = dynamic_pointer_cast<SphereCollider>(sharedOther))
        {
            float dist = (sharedOtherOwner->GetWorldPosition() - sharedOwner->GetWorldPosition()).norm();
            retval.isOverlapp = retval.isBlockingHit = dist < this->radius + sharedOtherSphere->radius;
            if (retval.isOverlapp)
            {
                retval.hitCollider = other;
                retval.impactNormal = (sharedOwner->GetWorldPosition() - sharedOtherOwner->GetWorldPosition()).normalized();
                // 球同士の中心を結んだ線上で球同士がオーバーラップしている線分の長さ
                retval.penetrationDepth = fmax(0, this->radius + sharedOtherSphere->radius - dist) * 0.5;
                // 片方の球の中心から衝突地点の中心までの距離
                float thisCenterToOverlappCenterLength = this->radius - retval.penetrationDepth * 0.5;
                // 衝突地点の計算
                retval.impactPoint = sharedOwner->GetWorldPosition() + -retval.impactNormal * thisCenterToOverlappCenterLength;
                retval.hitActor = sharedOtherOwner;
            }
        }
        // ボックスとの衝突の場合
        else if (auto sharedOtherBox = dynamic_pointer_cast<BoxCollider>(sharedOther))
        {
            BoundingBox3D box(sharedOtherBox->size);
            Matrix4f invModelMat = sharedOtherOwner->getWorldMat().inverse();
            float sphereRadius = this->radius;
            Vector3f tmp = sharedOwner->GetWorldPosition();
            Vector3f fixedSphereCentor = (invModelMat * Vector4f(tmp.x(), tmp.y(), tmp.z(), 1)).head<3>();

            retval.isOverlapp = retval.isBlockingHit = box.IsOverlapp(fixedSphereCentor, sphereRadius);
            if (retval.isOverlapp)
            {
                retval.hitCollider = other;
                Vector3f normalOS = box.ComputeNearestBoxFaceNormal(fixedSphereCentor);

                retval.impactNormal = Affine3f(sharedOtherOwner->getWorldMat()).rotation() * normalOS;
                retval.penetrationDepth = fmax(0, box.ComputePenetrationDist(fixedSphereCentor, sphereRadius));

                // ボックス上の最近傍点をヒット点にする
                Vector3f boxNearestPointOS = box.ComputeNearestBoxFacePos(fixedSphereCentor);
                retval.impactPoint = (sharedOtherOwner->getWorldMat() * Vector4f(boxNearestPointOS.x(), boxNearestPointOS.y(), boxNearestPointOS.z(), 1)).head<3>();
                retval.hitActor = sharedOtherOwner;
            }
        }
        retval.otherMaterial = sharedOther->physicMaterial;
        retval.thisMaterial = this->physicMaterial;
        retval.penetrationDepth = fmin(this->radius, retval.penetrationDepth);
        return retval;
    }
    float SphereCollider::GetBoundingRadius() const
    {
        return this->radius;
    }
    void SphereCollider::CaluculateMass()
    {
        // pi*r^2で体積を求め体積を更新
        if (this->isAutoCaluculateMassFromVolume)
            this->mass = this->radius * this->radius * M_PI * this->physicMaterial.density;
    }
    void SphereCollider::DrawDebugShape()
    {
        Collider::DrawDebugShape();
        if (auto sharedOwner = this->owner.lock())
            DebugDrawSubSystem::getInstance().DrawDebugSphere(
                this->radius,
                sharedOwner->GetWorldPosition(),
                sharedOwner->GetWorldRotation(),
                sharedOwner->GetWorldScale() * radius,
                this->colliderDebugColor);
    }

}
