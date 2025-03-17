#pragma once

#include "../Actor.hpp"
#include "../Scene.hpp"
#include "RandomWalkPointLightActor.hpp"
namespace REngine::Light
{
    /// @brief ライトをランダムにスポーンする
    class LightSpawnActor : public Actor
    {
    public:
        virtual void BeginPlay();
        LightSpawnActor(const json &params);
    };

    GCLASS(LightSpawnActor)
}