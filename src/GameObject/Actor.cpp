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
    this->mat = MakeMatOffset(this->location) * MakeMatScale(this->scale) * MakeRotMatZ(this->rotation.z()) * MakeRotMatX(this->rotation.x()) * MakeRotMatY(this->rotation.y());
    if (auto locked = parent.lock())
        this->mat = locked->mat * this->mat;
}
void Actor::SetParent(weak_ptr<Actor> parent)
{
    this->parent.reset();
    this->matUpdate();
}
void Actor::DettachParent()
{
    this->parent.reset();
    this->matUpdate();
}
void Actor::AddChild(weak_ptr<Actor> child)
{
    this->children.push_back(child);
    if (auto locked = child.lock())
        locked->SetParent(dynamic_pointer_cast<Actor>(this->shared_from_this()));
}
void Actor::DettachChild(weak_ptr<Actor> child)
{
    for (int i = 0; i < this->children.size(); i++)
    {
        if (this->children[i].lock() == child.lock())
        {
            this->children.erase(this->children.begin() + i);
            return;
        }
    }
}

Matrix4f Actor::getMat()
{
    return this->mat;
}