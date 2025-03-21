#pragma once
#include "../ActorComponent.hpp"
#include "../../../Engine/Data/Hit.hpp"
#include "../../../Math/BoundingBox3D.hpp"
#include "../../../Engine/SubSystem/DebugDrawSubSystem.hpp"
namespace REngine::Component
{
    /// @brief メッシュを持つアクター
    class Collider : public ActorComponent
    {
    protected:
        Vector3f colliderDebugColor = Vector3f::UnitX();

    public:
        float mass = 1.0f;                          // 質量(質量の自動計算が無効の場合に使用)
        float dencity = 1.0f;                       // 密度(質量の自動計算時に使用)
        bool isAutoCaluculateMassFromVolume = true; // 体積と密度から体積を自動計算するかどうか
        float restitution = 0.8f;                   // 反発係数

        virtual Hit DetectHit(const weak_ptr<Collider> &other) = 0;
        virtual void CaluculateMass();

        virtual vector<Hit> GetAllOverlappingActor();

        virtual void DrawDebugShape();

        virtual void BeginPlay() override;
        virtual void Tick(float deltasecond) override;
    };
    // GCLASS(Collider)
}