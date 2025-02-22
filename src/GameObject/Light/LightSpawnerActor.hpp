#pragma once

#include "../Actor.hpp"
#include "../Scene.hpp"
#include "RandomWalkPointLightActor.hpp"
using namespace Transform;

/// @brief ライトをランダムにスポーンする
class LightSpawnActor : public Actor
{
public:
    virtual void BeginPlay();
    LightSpawnActor(const json &params);
};

GCLASS(LightSpawnActor)
