#pragma once

#include "LightBaseActor.hpp"
#include "../../header/RenderingHeader.hpp"

namespace REngine::Light
{
    using namespace Transform;

    /// @brief ポイントライトのクラス
    class TorusLightActor : public LightBaseActor
    {
    protected:
        float majorRadius = 3.0f;
        float minorRadius = 1.0f;

    public:
        virtual Vector3f lightSDF(const Vector3f &positionWS, const Vector3f &normalWS);
        virtual float GetBoundingSphereRadius();
        TorusLightActor(json args);
        TorusLightActor() {}
        ~TorusLightActor();
    };
    GCLASS(TorusLightActor)
}