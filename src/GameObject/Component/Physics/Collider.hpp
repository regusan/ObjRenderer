#pragma once
#include "../ActorComponent.hpp"
#include "../../../Engine/Data/Hit.hpp"
#include "../../../Engine/Data/PhysicMaterial.hpp"
#include "../../../Math/BoundingBox3D.hpp"
#include "../../../Engine/SubSystem/DebugDrawSubSystem.hpp"
namespace REngine::Component
{
    /// @brief メッシュを持つアクター
    class Collider : public ActorComponent
    {
    protected:
        Vector3f colliderDebugColor = Vector3f::UnitX();
        vector<Hit> collisionChache;
        float lastCollisionChachedTime = 0;

    public:
        float mass = 1.0f;                          // 質量(質量の自動計算が無効の場合に使用)
        bool isAutoCaluculateMassFromVolume = true; // 体積と密度から体積を自動計算するかどうか

        char collisionLayer =
            0b0000001; // 自身のコリジョンレイヤー (デフォルト: 0x1)
        char hitLayerMask =
            0b1111111; // ヒット判定対象のレイヤーマスク (デフォルト: 全て)
        char overlapLayerMask =
            0b1111111; // オーバーラップ判定対象のレイヤーマスク (デフォルト: 全て)

        PhysicMaterial physicMaterial = PhysicMaterialExample::Glass;

        virtual Hit DetectHit(const weak_ptr<Collider> &other) = 0;
        virtual void CaluculateMass();

        virtual vector<Hit> GetAllOverlappingActor();
        bool IsValidLayer(const weak_ptr<Collider> &other) const;
        bool IsHitWith(const weak_ptr<Collider> &other) const;
        virtual float GetBoundingRadius() const = 0;
        bool IsOverlapWith(const weak_ptr<Collider> &other) const;

        virtual void DrawDebugShape();

        virtual void BeginPlay() override;
        virtual void Tick(float deltasecond) override;
    };
    // GCLASS(Collider)
}