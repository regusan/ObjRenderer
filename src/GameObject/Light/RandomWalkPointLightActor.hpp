#pragma once

#include "PointLightActor.hpp"
#include "../../header/RenderingHeader.hpp"
namespace REngine::Light
{
    /// @brief ポイントライトのクラス
    class RandomWalkPointLightActor : public PointLightActor
    {
    protected:
        float estTime = 0;
        Vector3f origin;
        Vector3f freq = Vector3f::Ones();

    public:
        RandomWalkPointLightActor();
        RandomWalkPointLightActor(json args);
        RandomWalkPointLightActor(Vector3f pos, float radius, Vector3f color);
        ~RandomWalkPointLightActor();
        virtual void Tick(float deltatime);
        virtual void BeginPlay();
    };
    GCLASS(RandomWalkPointLightActor)
}