#pragma once

#include "Actor.hpp"
#include "../header/RenderingHeader.hpp"
using namespace Transform;

/// @brief メッシュを持つアクター
class MeshActor : public Actor
{
private:
public:
    Model meshModel;
    MeshActor(json args);
    ~MeshActor();
};
GCLASS(MeshActor)
