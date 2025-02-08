#include "Mesh.hpp"

MeshActor::MeshActor(json args) : Actor(args)
{
    string pathStr = args["MeshPath"];
    this->meshModel.LoadModelFromFile(pathStr);
}
MeshActor::~MeshActor() {}