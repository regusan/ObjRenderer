#include "ActorComponent.hpp"
namespace REngine::Component
{
    ActorComponent::ActorComponent()
    {
    }
    ActorComponent::ActorComponent(json args)
    {
    }
    ActorComponent::~ActorComponent()
    {
    }

    void ActorComponent::SetOwner(const weak_ptr<Actor> owner)
    {
        this->owner = owner;
    }

    void ActorComponent::SetActive(const bool active)
    {
        this->bIsActive = active;
    }
    bool ActorComponent::GetActive() const
    {
        return this->bIsActive;
    }
}