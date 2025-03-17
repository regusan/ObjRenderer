#pragma once

#include "LightBaseActor.hpp"
#include "../../header/RenderingHeader.hpp"
using namespace Transform;
namespace REngine::Light
{
    /// @brief エリアライトのクラス
    class AreaLightActor : public LightBaseActor
    {
    protected:
        float length = 3.0f;
        float barnDoorAngle = 45.0f;
        float falloff = 0.1f;
        Vector2f size = Vector2f(1, 1);

    public:
        virtual Vector3f lightSDF(const Vector3f &positionWS, const Vector3f &normalWS);
        virtual float GetBoundingSphereRadius();
        AreaLightActor(json args);
        ~AreaLightActor();
    };
    GCLASS(AreaLightActor)
}