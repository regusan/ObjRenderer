#pragma once

#include "LightBaseActor.hpp"
#include "../../header/RenderingHeader.hpp"
using namespace Transform;

/// @brief ポイントライトのクラス
class PointLightActor : public LightBaseActor
{
protected:
    float radius = 3.0f;

public:
    virtual Vector3f lightSDF(const Vector3f &positionWS, const Vector3f &normalWS);
    virtual float GetBoundingSphereRadius();
    PointLightActor(json args);
    ~PointLightActor();
};
GCLASS(PointLightActor)
