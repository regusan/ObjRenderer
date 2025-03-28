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
        bool culled = false;
        shared_ptr<Model> meshModel = nullptr;
        MeshActor();
        MeshActor(json args);
        ~MeshActor();
        virtual void toString(ostream &os) const;
    };
    GCLASS(MeshActor)
}