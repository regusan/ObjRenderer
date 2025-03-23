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
    void RigitBody::BeginPlay()
    {
        ActorComponent::BeginPlay();
        this->lastActiveTime = this->sceneContext->timeManager.GetCurrentTime();
        this->isSleeping = false;
        if (auto unlockedOwner = owner.lock())
            this->lastPosition = unlockedOwner->GetWorldPosition();
    }
    vector<Hit> RigitBody::GetHits(const vector<weak_ptr<Collider>> &colliders)
    {
        vector<Hit> hits;
        for (auto &collider : colliders)
        {
            if (auto sp = collider.lock())
            {
                auto newHits = sp->GetAllOverlappingActor();
                hits.insert(hits.end(), newHits.begin(), newHits.end());
            }
        }
        return hits;
    }
    Hit RigitBody::GetPrimaryHit(const vector<Hit> &hits)
    {
        Hit primary;
        for (const auto &hit : hits)
        {
            if ((hit.isBlockingHit || hit.isOverlapp) && hit.penetrationDepth > primary.penetrationDepth)
            {
                primary = hit;
            }
        }
        return primary;
    }
    Vector3f RigitBody::GetProjectedGravityAcceleration(Vector3f floorNormal) const
    { // 重力の処理を改善
        Vector3f projectedGravityDirection = this->gravityDirection;

        Vector3f normalizedHitNormal = floorNormal.normalized();
        float dotProduct = this->gravityDirection.dot(normalizedHitNormal);

        // 斜面に沿った重力の投影を滑らかに
        if (dotProduct < -0.9f)
        {
            // ほぼ水平な面上にいる場合、重力をほぼキャンセル
            projectedGravityDirection = Vector3f::Zero();
        }
        else if (dotProduct > 0.1f)
        {
            // 天井に近い面に接触している場合、重力をそのまま適用
            projectedGravityDirection = this->gravityDirection;
        }
        else
        {
            // 斜面に沿った重力の計算（スムーズに）
            Vector3f projectionNormal = normalizedHitNormal;
            projectedGravityDirection = this->gravityDirection -
                                        projectionNormal * (this->gravityDirection.dot(projectionNormal));

            // 急な斜面での滑りを制御
            float slopeAngle = std::acos(std::abs(dotProduct));
            float maxSlopeAngle = 0.5f; // ラジアン単位、約30度

            if (slopeAngle > maxSlopeAngle)
            {
                // 急な斜面では重力の影響を強める
                float slopeFactor = (slopeAngle - maxSlopeAngle) / (M_PI / 2 - maxSlopeAngle);
                projectedGravityDirection *= (1.0f + slopeFactor);
            }
        }

        return projectedGravityDirection * this->gravityScale;
    }

    Vector3f RigitBody::GetAccumulatedPushBack(const vector<Hit> &hits)
    {
        Vector3f pushBackAccumulated = Vector3f::Zero();
        for (const auto &hit : hits)
        {
            pushBackAccumulated += hit.penetrationDepth * hit.impactNormal;
        }
        return pushBackAccumulated;
    }
    void RigitBody::Tick(float deltaTime)
    {
        ActorComponent::Tick(deltaTime);
        if (!this->GetActive())
            return;

        auto unlockedOwner = owner.lock();
        if (!unlockedOwner)
            return;

        // コライダーの収集
        vector<weak_ptr<Collider>> colliders = unlockedOwner->GetComponentsOfClass<Collider>();

        // 全ての衝突を収集
        vector<Hit> hits = this->GetHits(colliders);

        Hit primaryHit = this->GetPrimaryHit(hits);
        auto thisCollider = unlockedOwner->GetComponentOfClass<Collider>().lock();

        // 衝突処理
        Vector3f hitNormalAccumulated = Vector3f::Zero();
        int hitCount = 0;
        int overlappCount = 0;
        float maxPenetrationDepth = 0; // intからfloatに変更

        for (auto &hit : hits)
        {
            auto hitCollider = hit.hitCollider.lock();
            auto thisCollider = hit.thisCollider.lock();
            if (!hitCollider || !thisCollider)
                continue;

            if (hit.isBlockingHit) // 衝突があったら
            {
                hitCount++;
                // 貫通深度に基づく押し戻し力（より滑らかな係数を適用）
                // pushBackAccumulated += hit.impactNormal * hit.penetrationDepth * 1.5f;
            }

            if (hit.isBlockingHit && hit.isOverlapp)
            {
                overlappCount++;
                if (maxPenetrationDepth < hit.penetrationDepth)
                {
                    maxPenetrationDepth = hit.penetrationDepth;
                    // 反発係数を両オブジェクトの平均に変更
                    float combinedBounciness = (thisCollider->physicMaterial.bounciness + hitCollider->physicMaterial.bounciness) * 0.5f;
                    hitNormalAccumulated = hit.impactNormal * combinedBounciness;
                }
            }
        }

        // 反射と摩擦の処理
        if (!hits.empty())
        {
            // 複数のヒットがある場合、平均を計算
            // hitNormalAccumulated /= std::max(1, overlappCount);
            // pushBackAccumulated /= std::max(1, hitCount);

            // 加速度の蓄積を滑らかに
            float accelerationBlend = 0.8f; // 前フレームからの影響を減らす
            accumulatedAcceleration = accumulatedAcceleration * (1.0f - accelerationBlend) + hitNormalAccumulated * accelerationBlend;

            if (accumulatedAcceleration.norm() > 0.001f)
            {
                Vector3f normalizedVelocity = this->velocity.normalized();
                Vector3f normalizedAccel = accumulatedAcceleration.normalized();

                // 衝突面に対する摩擦の計算を追加
                PhysicMaterial resolved = primaryHit.thisMaterial.Resolve(primaryHit.otherMaterial);

                float normalComponent = this->velocity.dot(normalizedAccel);
                Vector3f tangentialVelocity = this->velocity - normalizedAccel * normalComponent;

                // 摩擦を適用
                tangentialVelocity *= (1.0f - resolved.dynamicFriction);

                // 反射（正の法線成分のみ反射）
                Vector3f reflectedVelocity = MathPhysics::Reflect(normalizedVelocity, normalizedAccel);
                this->velocity = reflectedVelocity * this->velocity.norm() * std::max(0.2f, accumulatedAcceleration.norm());

                // 摩擦による減速を加味
                this->velocity += tangentialVelocity * resolved.dynamicFriction;
            }
        }
        Vector3f fixedGravityAcceleration = this->gravityDirection * this->gravityScale;

        if (hitCount > 0)
        {
            fixedGravityAcceleration = this->GetProjectedGravityAcceleration(hitNormalAccumulated.normalized());
        }
        // 重力と速度減衰の適用
        this->velocity += fixedGravityAcceleration * deltaTime;
        this->velocity *= (1.0f - this->linearDamping * deltaTime);

        float boundingRadius = 0;
        if (thisCollider)
            boundingRadius = thisCollider->GetBoundingRadius();

        Vector3f pushBackAccumulated = this->GetAccumulatedPushBack(hits);
        float minPushbackThreshold = boundingRadius * 0.05f; // 閾値を5%に下げる
        if (pushBackAccumulated.norm() > minPushbackThreshold)
        {
            // より滑らかなプッシュバックを適用
            float pushStrength = std::min(1.0f, pushBackAccumulated.norm() / boundingRadius);
            pushBackAccumulated *= pushStrength;
        }
        else
        {
            pushBackAccumulated = Vector3f::Zero();
        }

        // 最終的な移動計算
        Vector3f moveOffset = this->velocity * deltaTime + pushBackAccumulated;
        moveOffset = moveOffset.array() *
                     (Vector3i::Ones() - this->positionConstraint).cast<float>().array(); // 拘束軸の処理

        // 位置の更新
        this->lastPosition = unlockedOwner->GetWorldPosition();
        unlockedOwner->SetLocalPosition(unlockedOwner->GetWorldPosition() + moveOffset);

        this->UpdateSleepState();
    }
    void RigitBody::UpdateSleepState()
    {
        auto unlockedOwner = owner.lock();
        // スリープ状態の管理
        if ((this->lastPosition - unlockedOwner->GetWorldPosition()).norm() > 0.001f)
        {
            this->lastActiveTime = this->sceneContext->timeManager.GetCurrentTime();
            this->isSleeping = false;
        }
        else if (this->sceneContext->timeManager.GetCurrentTime() - this->lastActiveTime > this->sleepThreshold)
        {
            this->isSleeping = true;
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