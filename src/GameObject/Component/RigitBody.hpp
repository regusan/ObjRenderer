#pragma once
#include "ActorComponent.hpp"

#include "Physics/Collider.hpp"
namespace REngine::Component
{
    /// @brief メッシュを持つアクター
    class RigitBody : public ActorComponent
    {
    protected:
        Vector3f lastPosition = Vector3f::Zero();
        bool isSleeping = false;
        float sleepThreshold = 3.0f;
        float lastActiveTime = 0.0f;
        Vector3f accumulatedAcceleration = Vector3f::Zero();
        Vector3f accumulatedAngularAcceleration = Vector3f::Zero();
        Vector3f accumulatedPushBack = Vector3f::Zero();

        void UpdateSleepState();

        static vector<Hit> GetHits(const vector<weak_ptr<Collider>> &colliders);
        static Hit GetPrimaryHit(const vector<Hit> &hits);
        static Vector3f GetAccumulatedPushBack(const vector<Hit> &hits);
        Vector3f GetProjectedGravityAcceleration(Vector3f floorNormal) const;

    public:
        Vector3f velocity = Vector3f::Zero();

        Vector3f angularVelocity = Vector3f::Zero(); // 角速度

        /*重力系*/
        float gravityScale = 9.70;                      // 重力の大きさ
        bool enableGravity = true;                      // 重力が有効かどうか
        Vector3f gravityDirection = Vector3f(0, -1, 0); // 重力の向き

        float linearDamping = 0.1f;  // 移動の減衰率（空気抵抗）
        float angularDamping = 0.1f; // 回転の減衰率

        Vector3i positionConstraint = Vector3i::Zero(); // 位置を固定軸
        Vector3i rotationConstraint = Vector3i::Zero(); // 回転の固定軸

        RigitBody();
        RigitBody(json args);
        ~RigitBody();

        virtual void Tick(float deltaTime) override;
        virtual void BeginPlay() override;

        virtual void SetVelocity(const Vector3f &newVelocity);
        virtual void AddImpulse(const Vector3f &acceleration);

        void AddTorque(const Vector3f &torque);
        void SetAngularVelocity(const Vector3f &newAngularVelocity);

        bool IsSleeping() const;
    };
    GCLASS(RigitBody)
}