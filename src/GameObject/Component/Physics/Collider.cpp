#include "Collider.hpp"
namespace REngine::Component
{
    vector<Hit> Collider::GetAllOverlappingActor()
    {
        vector<Hit> retval;
        auto colliders = this->sceneContext->GetObjectsOfClass<Collider>();
        for (auto collider : colliders)
        {
            if (collider.lock() == shared_from_this())
                continue;
            auto result = this->DetectHit(collider);
            if (result.isOverlapp)
                retval.push_back(result);
        }
        return retval;
    }
    void Collider::CaluculateMass()
    {
    }
    void Collider::BeginPlay()
    {
        this->CaluculateMass();
    }
}