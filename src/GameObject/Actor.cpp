#include "Actor.hpp"

Actor::Actor(/* args */) {}
Actor::Actor(json args)
{
    json positionArg = args["position"];
    json rotationArg = args["rotation"];
    json scaleArg = args["scale"];
    this->SetPosition(Vector3f(positionArg[0], positionArg[1], positionArg[2]));
    this->SetRotation(Vector3f(rotationArg[0], rotationArg[1], rotationArg[2]));
    this->SetScale(Vector3f(scaleArg[0], scaleArg[1], scaleArg[2]));
    this->matUpdate();
}
Actor::~Actor() {}

void Actor::SetRotation(Vector3f rotate)
{
    this->rotation = rotate;
    this->matUpdate();
}
void Actor::SetPosition(Vector3f position)
{
    this->location = position;
    this->matUpdate();
}
void Actor::SetScale(Vector3f scale)
{
    this->scale = scale;
    this->matUpdate();
}

void Actor::matUpdate()
{
    this->mat = MakeMatScale(this->scale) * MakeRotMatX(this->rotation.z()) * MakeRotMatY(this->rotation.y()) * MakeRotMatY(this->rotation.x()) * MakeMatOffset(this->location);
}
Matrix4f Actor::getMat()
{
    return this->mat;
}