#pragma once
#include "../Component/RigitBody.hpp"
#include "../Component/Physics/BoxCollider.hpp"
namespace REngine::Collider
{
    /// @brief メッシュを持つアクター
    class BoxColliderActor : public Actor
    {
    private:
        Vector3f size = Vector3f::Ones();

    public:
        shared_ptr<Component::BoxCollider> boxCollider = nullptr;
        BoxColliderActor();
        BoxColliderActor(const Vector3f size);
        BoxColliderActor(json args);
        ~BoxColliderActor();
        virtual void BeginPlay() override;
        virtual void Tick(float deltaTime) override;
    };
    GCLASS(BoxColliderActor)
}