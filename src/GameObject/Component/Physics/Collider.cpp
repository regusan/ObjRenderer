#include "Collider.hpp"
namespace REngine::Component
{
    vector<Hit> Collider::GetAllOverlappingActor()
    {
        if (!this->GetActive())
            return vector<Hit>();
        // このティックですでに探索済みだったらキャッシュを返す
        if (this->lastCollisionChachedTime == this->sceneContext->timeManager.GetCurrentTime())
            return this->collisionChache;

        this->collisionChache.clear();
        this->lastCollisionChachedTime = this->sceneContext->timeManager.GetCurrentTime();

        auto colliders = this->sceneContext->GetObjectsOfClass<Collider>();
        for (auto collider : colliders)
        {
            if (collider.lock() == shared_from_this())
                continue;
            auto result = this->DetectHit(collider);
            if (result.isOverlapp)
                this->collisionChache.push_back(result);
        }
        return this->collisionChache;
    }
    bool Collider::IsValidLayer(const weak_ptr<Collider> &other) const
    {
        auto sharedOther = other.lock();
        if (!sharedOther)
            return (this->collisionLayer & sharedOther->hitLayerMask || this->collisionLayer & sharedOther->overlapLayerMask);
        return false;
    }
    bool Collider::IsHitWith(const weak_ptr<Collider> &other) const
    {
        auto sharedOther = other.lock();
        if (!sharedOther)
            return this->collisionLayer & sharedOther->hitLayerMask;
        return false;
    }
    bool Collider::IsOverlapWith(const weak_ptr<Collider> &other) const
    {
        auto sharedOther = other.lock();
        if (!sharedOther)
            return this->collisionLayer & sharedOther->overlapLayerMask;
        return false;
    }
    void Collider::CaluculateMass()
    {
    }
    void Collider::BeginPlay()
    {
        this->CaluculateMass();
    }
    void Collider::Tick(float deltasecond)
    {
        if (this->bIsActive)
            this->DrawDebugShape();
    }
    void Collider::DrawDebugShape()
    {
    }
}