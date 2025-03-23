
#include "BoxColliderActor.hpp"

namespace REngine::Collider
{
    BoxColliderActor::BoxColliderActor()
    {
    }
    BoxColliderActor::BoxColliderActor(const Vector3f size) : size(size)
    {
    }
    BoxColliderActor::BoxColliderActor(json args) : Actor::Actor(args)
    {
        this->size = Vector3f(args["size"][0], args["size"][1], args["size"][2]);
    }
    BoxColliderActor::~BoxColliderActor()
    {
    }
    void BoxColliderActor::BeginPlay()
    {
        this->boxCollider = this->sceneContext->SpawnActorOfClass<Component::BoxCollider>(this->size).lock();
        this->AddComponent(this->boxCollider);
    }
    void BoxColliderActor::Tick(float deltaTime)
    {
    }
}
