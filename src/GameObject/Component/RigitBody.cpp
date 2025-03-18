#include "RigitBody.hpp"
#include "../Actor.hpp"
namespace REngine::Component
{

    RigitBody::RigitBody()
    {
    }
    RigitBody::RigitBody(json args)
    {
    }
    RigitBody::~RigitBody()
    {
    }
    void RigitBody::Tick(float deltaTime)
    {
        ActorComponent::Tick(deltaTime);
        this->velocity += this->gravityDirection * this->gravityScale * deltaTime;
        this->velocity *= (1 - this->linearDamping * deltaTime);
        if (auto unlocked = owner.lock())
        {
            unlocked->SetLocalPosition(unlocked->location + this->velocity * deltaTime);
        }
    }

    void RigitBody::AddImpulse(const Vector3f acceleration)
    {
        this->velocity += acceleration;
    }
    void RigitBody::SetVelocity(const Vector3f newVelocity)
    {
        this->velocity = newVelocity;
    }

    void RigitBody::AddTorque(const Vector3f &torque)
    {
        this->angularVelocity += torque;
    }
    void RigitBody::SetAngularVelocity(const Vector3f &newAngularVelocity)
    {
        this->angularVelocity = newAngularVelocity;
    }
}