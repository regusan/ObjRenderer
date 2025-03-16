#include "Mesh.hpp"

using namespace REngine;
MeshActor::MeshActor() : Actor()
{
}

MeshActor::MeshActor(json args) : Actor(args)
{
    string meshPath = args["MeshPath"];
    this->meshModel = AssetSubSystem::getInstance().meshManager.LoadAsset(meshPath);
    filesystem::path dir = filesystem::path(meshPath).parent_path();
    string VATPosPath = args.value("VATPosPath", "");
    if (VATPosPath.length() > 0)
        this->meshModel->VATPos = AssetSubSystem::getInstance().textureManager.LoadAsset(dir / VATPosPath);
    string VATNormalPath = args.value("VATNormalPath", "");
    if (VATNormalPath.length() > 0)
        this->meshModel->VATNormal = AssetSubSystem::getInstance().textureManager.LoadAsset(dir / VATNormalPath);
}
MeshActor::~MeshActor() {}