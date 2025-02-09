#pragma once

#include "LightBaseActor.hpp"
#include "../../header/RenderingHeader.hpp"
using namespace Transform;

/// @brief ポイントライトのクラス
class PointLightActor : public LightBaseActor
{
private:
    float radius = 3.0f;

public:
    virtual Vector3f lightSDF(const Vector3f &positionWS, const Vector3f &normalWS);
    PointLightActor(json args);
    ~PointLightActor();
};
GCLASS(PointLightActor)
