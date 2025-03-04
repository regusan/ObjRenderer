#include "Mesh.hpp"

MeshActor::MeshActor(json args) : Actor(args)
{
    string meshPath = args["MeshPath"];
    this->meshModel.LoadModelFromFile(meshPath);
    filesystem::path dir = filesystem::path(meshPath).parent_path();
    string VATPosPath = args.value("VATPosPath", "");
    this->meshModel.VATPos = AssetSubSystem::getInstance().textureManager.LoadAsset(dir / VATPosPath);
    string VATNormalPath = args.value("VATNormalPath", "");
    this->meshModel.VATNormal = AssetSubSystem::getInstance().textureManager.LoadAsset(dir / VATNormalPath);
}
MeshActor::~MeshActor() {}