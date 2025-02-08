#include "GameObject.hpp"
GameObject::GameObject(/* args */)
{
    this->uuid = random();
    this->name = this->uuid;
}

GameObject::~GameObject()
{
}

void GameObject::Tick(float deltatime) {}
void GameObject::BeginPlay() {}

string GameObject::GetObjectName()
{
    return this->name;
}