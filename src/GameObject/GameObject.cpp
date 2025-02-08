#include "GameObject.hpp"
GameObject::GameObject(/* args */)
{
    this->uuid = random();
    this->name = this->uuid;
}

GameObject::~GameObject()
{
}

string GameObject::GetObjectName()
{
    return this->name;
}