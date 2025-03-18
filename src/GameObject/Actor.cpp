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
        this->SetLocalPosition(Vector3f(positionArg[0], positionArg[1], positionArg[2]));
        this->SetLocalRotation(Vector3f(rotationArg[0], rotationArg[1], rotationArg[2]));
        this->SetLocalScale(Vector3f(scaleArg[0], scaleArg[1], scaleArg[2]));
        this->matUpdate();
    }
    Actor::~Actor() {}
    void Actor::OnDestroyed()
    {
        for (auto child : this->children)
            this->sceneContext->DestroyObject(child);
    }

    // Transform系
    void Actor::SetLocalRotation(Vector3f rotate)
    {
        this->rotation = rotate;
        this->matUpdate();
    }
    void Actor::SetLocalPosition(Vector3f position)
    {
        this->location = position;
        this->matUpdate();
    }
    void Actor::SetLocalScale(Vector3f scale)
    {
        this->scale = scale;
        this->matUpdate();
    }

    /*
        void Actor::SetWorldRotation(Vector3f rotate)
    {
        if (auto parentActor = dynamic_pointer_cast<Actor>(parent.lock()))
        {
            Vector3f localRotation = Affine3f(this->localMatrix).rotation().eulerAngles(2, 0, 1);
            Affine3f(this->localMatrix).tra
            SetLocalRotation(localRotation);
        }
        else
        {
            SetLocalRotation(rotate);
        }
        this->matUpdate();
    }
    void Actor::SetWorldPosition(Vector3f position)
    {
        if (auto parentActor = dynamic_pointer_cast<Actor>(parent.lock()))
        {
            Matrix4f parentInv = parentActor->getWorldMat().inverse();
            Vector3f localPosition = parentInv.topRightCorner<3, 1>() + position;
            SetLocalPosition(localPosition);
        }
        else
        {
            SetLocalPosition(position);
        }
        this->matUpdate();
    }
    void Actor::SetWorldScale(Vector3f scale)
    {
        if (auto parentActor = dynamic_pointer_cast<Actor>(parent.lock()))
        {
            Vector3f parentScale = parentActor->scale;
            Vector3f localScale = scale.cwiseQuotient(parentScale);
            SetLocalScale(localScale);
        }
        else
        {
            SetLocalScale(scale);
        }
        this->matUpdate();
    }
    */

    Vector3f Actor::GetWorldRotation() const
    {
        if (auto actor = parent.lock())
            return this->rotation + actor->GetWorldRotation();
        return this->rotation;
    }

    Vector3f Actor::GetWorldPosition() const
    {
        return Affine3f(this->getWorldMat()).translation();
    }

    Vector3f Actor::GetWorldScale() const
    {
        return Affine3f(this->getWorldMat()).linear().colwise().norm();
    }

    Vector3f Actor::GetLocalRotation() const
    {
        Matrix3f localRotationMatrix = Affine3f(localMatrix).linear();
        localRotationMatrix.col(0).normalize();
        localRotationMatrix.col(1).normalize();
        localRotationMatrix.col(2).normalize();
        return localRotationMatrix.eulerAngles(2, 0, 1) * 180 / M_PI; // YXZ順でオイラー角に変換
    }

    Vector3f Actor::GetLocalPosition() const
    {
        return (Affine3f(localMatrix)).translation();
    }

    Vector3f Actor::GetLocalScale() const
    {
        return Affine3f(this->localMatrix).linear().colwise().norm();
    }

    void Actor::matUpdate()
    {
        this->localMatrix = this->worldMatrix =
            (Scaling(this->scale.x(), this->scale.y(), this->scale.z()) *
             Translation3f(this->location) *
             AngleAxisf(this->rotation.y() * M_PI / 180.0f, Vector3f::UnitY()) *
             AngleAxisf(this->rotation.x() * M_PI / 180.0f, Vector3f::UnitX()) *
             AngleAxisf(this->rotation.z() * M_PI / 180.0f, Vector3f::UnitZ()))
                .matrix();
        if (auto actor = parent.lock())
            this->worldMatrix = actor->worldMatrix * this->worldMatrix;
        for (auto child : children)
        {
            if (auto actor = child.lock())
            {
                actor->matUpdate();
            }
        }
    }

    Matrix4f Actor::getWorldMat() const
    {
        return this->worldMatrix;
    }

    void Actor::SetParent(weak_ptr<Actor> parent)
    {
        this->parent = parent;
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
        if (auto lockedChild = child.lock())
            lockedChild->SetParent(static_pointer_cast<Actor>(this->shared_from_this()));
    }
    void Actor::DettachChild(weak_ptr<Actor> child)
    {
        for (size_t i = 0; i < this->children.size(); i++)
        {
            if (this->children[i].lock() == child.lock())
            {
                this->children.erase(this->children.begin() + i);
                return;
            }
        }
    }

    void Actor::AddComponent(weak_ptr<Component> component)
    {
        this->components.push_back(component);
    }

    void Actor::RemoveComponent(weak_ptr<Component> component)
    {
        for (size_t i = 0; i < this->components.size(); i++)
        {
            if (this->components[i].lock() == component.lock())
            {
                this->components.erase(this->components.begin() + i);
                return;
            }
        }
    }
}