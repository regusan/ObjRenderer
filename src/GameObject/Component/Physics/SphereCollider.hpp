#pragma once
#include "Collider.hpp"

namespace REngine::Component
{
    /// @brief メッシュを持つアクター
    class SphereCollider : public Collider
    {
    private:
    public:
        float radius = 1.0f;

        SphereCollider();
        SphereCollider(const float radius);
        SphereCollider(json args);
        ~SphereCollider();
        virtual Hit DetectHit(const weak_ptr<Collider> &other) override;
        virtual void CaluculateMass() override;
    };
    GCLASS(SphereCollider)
}