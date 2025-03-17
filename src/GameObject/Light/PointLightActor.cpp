#include "PointLightActor.hpp"
namespace REngine::Light
{
    Vector3f PointLightActor::lightSDF(const Vector3f &positionWS, const Vector3f &normalWS)
    {
        // 球状に、かつ光源方向を向いている面を明るくする
        return color * powf(saturate(radius - (positionWS - this->location).norm()), 2) *
               saturate(normalWS.dot((this->location - positionWS).normalized()));
    }
    float PointLightActor::GetBoundingSphereRadius()
    {
        return this->radius;
    }
    PointLightActor::PointLightActor(json args) : LightBaseActor(args)
    {
        this->radius = args["radius"];
    }
    PointLightActor::~PointLightActor() {}
}