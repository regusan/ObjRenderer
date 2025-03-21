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
        if (!sharedOther && !sharedOwner)
            return Hit();
        auto sharedOtherOwner = sharedOther->owner.lock();
        if (!sharedOtherOwner)
            return Hit();

        Hit retval;

        return retval;
    }
}
