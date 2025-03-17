#pragma once

#include "Actor.hpp"
#include "../header/RenderingHeader.hpp"
namespace REngine
{
    /// @brief メッシュを持つアクター
    class MeshActor : public Actor
    {
    private:
    public:
        shared_ptr<Model> meshModel = nullptr;
        MeshActor();
        MeshActor(json args);
        ~MeshActor();
    };
    GCLASS(MeshActor)
}