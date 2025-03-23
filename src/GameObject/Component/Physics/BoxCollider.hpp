#pragma once
#include "Collider.hpp"

namespace REngine::Component
{
    /// @brief メッシュを持つアクター
    class BoxCollider : public Collider
    {
    private:
    public:
        Vector3f size = Vector3f::Ones();

        BoxCollider();
        BoxCollider(const Vector3f size);
        BoxCollider(json args);
        ~BoxCollider();
        virtual Hit DetectHit(const weak_ptr<Collider> &other) override;
        virtual float GetBoundingRadius() const override;
        virtual void CaluculateMass() override;
        virtual void DrawDebugShape() override;
    };
    GCLASS(BoxCollider)
}