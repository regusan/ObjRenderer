#include "PointLightActor.hpp"

Vector3f PointLightActor::lightSDF(const Vector3f &positionWS, const Vector3f &normalWS)
{
    // 球状に、かつ光源方向を向いている面を明るくする
    return color * clamp<float>(radius - (positionWS - this->location).norm(), 0, 1) * clamp<float>(normalWS.dot((this->location - positionWS).normalized()), 0, 1);
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