#pragma once

#include "PointLightActor.hpp"
#include "../../header/RenderingHeader.hpp"
namespace REngine::Light
{
    /// @brief エリアライトのクラス
    class CubeMapPointLight : public PointLightActor
    {
    protected:
        shared_ptr<Texture2D> map;

    public:
        virtual Vector3f lightSDF(const Vector3f &positionWS, const Vector3f &normalWS);
        virtual float GetBoundingSphereRadius();
        CubeMapPointLight(json args);
        ~CubeMapPointLight();
    };
    GCLASS(CubeMapPointLight)
}