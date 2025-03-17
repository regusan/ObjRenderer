#pragma once

#include "LightBaseActor.hpp"
#include "../../header/RenderingHeader.hpp"
namespace REngine::Light
{
    using namespace Transform;

    /// @brief エリアライトのクラス
    class SpotLightActor : public LightBaseActor
    {
    protected:
        float length = 3.0f;
        float angle = 90.0f;

    public:
        virtual Vector3f lightSDF(const Vector3f &positionWS, const Vector3f &normalWS);
        virtual float GetBoundingSphereRadius();
        SpotLightActor(json args);
        ~SpotLightActor();
    };
    GCLASS(SpotLightActor)
}