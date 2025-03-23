#pragma once
#include <memory>
#include "../../header/EigenHeader.hpp"
#include "PhysicMaterial.hpp"

namespace REngine::Component
{
    class Collider;
}
namespace REngine
{
    class Actor;
    class Hit
    {
    private:
        /* data */
    public:
        bool isBlockingHit;
        bool isOverlapp;
        Vector3f impactPoint;  // 衝突したワールド座標
        Vector3f impactNormal; // 衝突した面の法線
        float penetrationDepth;
        std::weak_ptr<Actor> hitActor;
        std::weak_ptr<Component::Collider> hitCollider;
        std::weak_ptr<Component::Collider> thisCollider;
        PhysicMaterial thisMaterial;
        PhysicMaterial otherMaterial;

        Hit(/* args */);
        ~Hit();
    };
}