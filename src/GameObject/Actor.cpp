#include "Actor.hpp"

Actor::Actor(/* args */) {}
Actor::~Actor() {}

void Actor::SetRotation(Vector3f rotate)
{
    this->rotation = rotate;
}
void Actor::SetPosition(Vector3f position)
{
    this->location = position;
}

void Actor::matUpdate()
{
    this->mat = MakeRotMatX(this->rotation.z()) * MakeRotMatY(this->rotation.y()) * MakeRotMatY(this->rotation.x()) * MakeMatOffset(this->location);
}
Matrix4f Actor::getMat()
{
    this->matUpdate();
    return this->mat;
}