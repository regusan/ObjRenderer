#include "GameObject.hpp"
GameObject::GameObject(/* args */)
{
    this->uuid = random();
    this->name = this->uuid;
}

GameObject::~GameObject()
{
}
void GameObject::SetSpawnedScene(Scene *_scene)
{
    this->sceneContext = _scene;
}

void GameObject::Tick(float deltatime) {}
void GameObject::BeginPlay() {}
void GameObject::OnDestroyed() {}

string GameObject::GetObjectName()
{
    return this->name;
}