#include "SpotLightActor.hpp"
namespace REngine::Light
{
    Vector3f SpotLightActor::lightSDF(const Vector3f &positionWS, const Vector3f &normalWS)
    {
        // 球状に、かつ光源方向を向いている面を明るくする
        Vector3f normal = Transform::GetForwardVector(this->getMat()).head<3>();

        Vector3f light2PosDir = (this->location - positionWS).normalized();
        float angleRate = this->angle / 180;
        float angleIntencity = saturate((light2PosDir.dot(normal) - (1 - angleRate)) / angleRate);
        float distant = powf(saturate(this->length - (positionWS - this->location).norm()), 2);
        float directant = saturate(normalWS.dot(light2PosDir));
        return color * angleIntencity * distant * directant;
    }
    float SpotLightActor::GetBoundingSphereRadius()
    {
        return this->length;
    }
    SpotLightActor::SpotLightActor(json args) : LightBaseActor(args)
    {
        this->length = args["radius"];
        this->angle = args["angle"];
    }
    SpotLightActor::~SpotLightActor() {}
}