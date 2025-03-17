#include "Actor.hpp"

namespace REngine
{
    using namespace Transform;
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
        if (auto actor = dynamic_pointer_cast<Actor>(parent.lock()))
            this->mat = actor->mat * this->mat;
        for (auto child : children)
        {
            if (auto actor = dynamic_pointer_cast<Actor>(child.lock()))
                actor->matUpdate();
        }
    }

    Matrix4f Actor::getMat()
    {
        return this->mat;
    }

    void Actor::SetParent(weak_ptr<Actor> parent)
    {
        GameObject::SetParent(parent);
        this->matUpdate();
    }
    void Actor::DettachParent()
    {
        GameObject::DettachParent();
        this->matUpdate();
    }
}