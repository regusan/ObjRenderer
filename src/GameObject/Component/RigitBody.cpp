#include "RigitBody.hpp"
#include "../Actor.hpp"
#include "Physics/Collider.hpp"

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
    void RigitBody::BeginPlay()
    {
        ActorComponent::BeginPlay();
        this->lastActiveTime = this->sceneContext->timeManager.GetCurrentTime();
        this->isSleeping = false;
        if (auto unlockedOwner = owner.lock())
            this->lastPosition = unlockedOwner->GetWorldPosition();
    }
    void RigitBody::Tick(float deltaTime)
    {
        ActorComponent::Tick(deltaTime);

        auto unlockedOwner = owner.lock();
        if (!unlockedOwner)
            return;

        vector<weak_ptr<Collider>> colliders;
        colliders = unlockedOwner->GetComponentsOfClass<Collider>();

        // 全ての衝突を収集
        vector<Hit> hits;
        for (auto &collider : colliders)
        {
            if (auto sp = collider.lock())
            {
                auto newHits = sp->GetAllOverlappingActor();
                hits.insert(hits.end(), newHits.begin(), newHits.end());
            }
        }

        // 衝突によって生じる力を計算
        Vector3f hitNormalAccumulated = Vector3f::Zero();
        Vector3f pushBackAccumulated = Vector3f::Zero();
        int hitCount = 0;
        int overlappCount = 0;
        for (auto &hit : hits)
        {
            auto hitCollider = hit.hitCollider.lock();
            auto thisCollider = hit.thisCollider.lock();
            if (hit.isBlockingHit && hitCollider) // 衝突があったら
            {
                hitCount++;
                pushBackAccumulated += hit.impactNormal * hit.penetrationDepth;
            }
            if (hit.isBlockingHit && hit.isOverlapp && hitCollider)
            {
                overlappCount++;
                hitNormalAccumulated += hit.impactNormal * fmin(thisCollider->restitution, hitCollider->restitution);
            }
        }

        // 何かしらの衝突があったら反射処理
        if (hits.size() != 0)
        { // TODO::とりあえず各衝突の平均をとる
            hitNormalAccumulated /= fmin(1, overlappCount);
            pushBackAccumulated /= fmin(1, hitCount);
            this->logStart(cout) << pushBackAccumulated << endl;
            accumulatedAcceleration = accumulatedAcceleration * 0.0 + hitNormalAccumulated * 0.9999;
            this->velocity = MathPhysics::Reflect(this->velocity, accumulatedAcceleration) * this->velocity.norm() * accumulatedAcceleration.norm(); // 反射
        }

        Vector3f projectedGravityDirection;                              // 斜面方向に投影された重力
        if (hitCount != 0)                                               // 地面に着地していたら
            if (this->gravityDirection.dot(hitNormalAccumulated) < -0.8) // あまりにも斜面と重力がついになっていたら発散を防ぐためゼロにする
                projectedGravityDirection = Vector3f::Zero();
            else
                projectedGravityDirection = this->gravityDirection - hitNormalAccumulated * (this->gravityDirection.dot(hitNormalAccumulated));
        else // 地面に着地していなければそのままの重力
            projectedGravityDirection = this->gravityDirection;

        this->velocity += projectedGravityDirection * this->gravityScale * deltaTime;

        this->velocity *= (1 - this->linearDamping * deltaTime);

        this->lastPosition = unlockedOwner->GetWorldPosition();
        unlockedOwner->SetLocalPosition(unlockedOwner->GetWorldPosition() + this->velocity * deltaTime + pushBackAccumulated * .0);

        if (this->lastPosition != unlockedOwner->GetWorldPosition())
        {
            this->lastActiveTime = this->sceneContext->timeManager.GetCurrentTime();
        }
        // 最後にアクティブになってから指定時間経過したらスリープに移行
        if (this->lastActiveTime - this->sceneContext->timeManager.GetCurrentTime() > this->sleepThreshold)
        {
            this->isSleeping = true;
        }
        else
        {
            this->isSleeping = false;
        }
    }

    void RigitBody::AddImpulse(const Vector3f &acceleration)
    {
        this->velocity += acceleration;
    }
    void RigitBody::SetVelocity(const Vector3f &newVelocity)
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

    bool RigitBody::IsSleeping() const
    {
        return this->isSleeping;
    }

}