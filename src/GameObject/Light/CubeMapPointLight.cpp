#include "CubeMapPointLight.hpp"
namespace REngine::Light
{
    Vector3f CubeMapPointLight::lightSDF(const Vector3f &positionWS, const Vector3f &normalWS)
    {
        if (!this->map)
        {
            return PointLightActor::lightSDF(positionWS, normalWS);
        }
        Vector3f light2PosDir = (this->location - positionWS).normalized();
        Vector4f light2PosDir4f = Vector4f(light2PosDir.x(), light2PosDir.y(), light2PosDir.z(), 1);
        Vector4f dirWS = Transform::ResetScale(Transform::ResetPosition(this->getWorldMat())) * light2PosDir4f;
        Vector2f uv = TextureMath::UVMod1(TextureMath::RectangularToPolarUV(dirWS.head<3>()));
        return PointLightActor::lightSDF(positionWS, normalWS).array() * map->SampleColor01(uv.x(), uv.y()).array();
    }
    float CubeMapPointLight::GetBoundingSphereRadius()
    {
        return this->radius;
    }
    CubeMapPointLight::CubeMapPointLight(json args) : PointLightActor(args)
    {
        this->map = AssetSubSystem::getInstance().textureManager.LoadAsset(args["MapPath"]);
    }
    CubeMapPointLight::~CubeMapPointLight() {}
}