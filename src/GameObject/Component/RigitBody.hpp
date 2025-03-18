#pragma once
#include "ActorComponent.hpp"

namespace REngine::Component
{
    /// @brief メッシュを持つアクター
    class RigitBody : public ActorComponent
    {
    private:
    public:
        float mass = 1.0f;

        Vector3f velocity = Vector3f::Zero();

        Vector3f angularVelocity = Vector3f::Zero(); // 角速度

        float gravityScale = 9.70;
        bool enableGravity = true;
        Vector3f gravityDirection = Vector3f(0, -1, 0);

        float linearDamping = 0.1f;  // 移動の減衰率（空気抵抗）
        float angularDamping = 0.1f; // 回転の減衰率

        RigitBody();
        RigitBody(json args);
        ~RigitBody();

        virtual void Tick(float deltaTime) override;

        virtual void SetVelocity(const Vector3f newVelocity);
        virtual void AddImpulse(const Vector3f acceleration);

        void AddTorque(const Vector3f &torque);
        void SetAngularVelocity(const Vector3f &newAngularVelocity);
    };
    GCLASS(RigitBody)
}