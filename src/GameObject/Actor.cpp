#include "Actor.hpp"
#include "Component/ActorComponent.hpp"

namespace REngine
{
    using namespace Transform;
    Actor::Actor(/* args */) {}
    Actor::Actor(json args)
    {
        json positionArg = args.value("position", json());

        json rotationArg = args.value("rotation", json());
        json scaleArg = args.value("scale", json());
        if (!positionArg.empty())
            this->SetLocalPosition(Vector3f(positionArg[0], positionArg[1], positionArg[2]));
        if (!rotationArg.empty())
            this->SetLocalRotation(Vector3f(rotationArg[0], rotationArg[1], rotationArg[2]));
        if (!scaleArg.empty())
            this->SetLocalScale(Vector3f(scaleArg[0], scaleArg[1], scaleArg[2]));
        this->matUpdate();
    }
    Actor::~Actor() {}
    void Actor::OnDestroyed()
    {
        for (auto child : this->children)
            this->sceneContext->DestroyObject(child);
        for (auto child : this->components)
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
            (Translation3f(this->location) *
             Scaling(this->scale.x(), this->scale.y(), this->scale.z()) *
             AngleAxisf(this->rotation.z() * M_PI / 180.0f, Vector3f::UnitZ()) *
             AngleAxisf(this->rotation.y() * M_PI / 180.0f, Vector3f::UnitY()) *
             AngleAxisf(this->rotation.x() * M_PI / 180.0f, Vector3f::UnitX()))
                .matrix();
        if (auto actor = parent.lock())
            this->worldMatrix = actor->worldMatrix * this->localMatrix;
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
        if (auto lockedParent = parent.lock())
            lockedParent->children.push_back(static_pointer_cast<Actor>(this->shared_from_this()));
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

    void Actor::AddComponent(weak_ptr<ActorComponent> component)
    {
        this->components.push_back(component);
        if (auto lockedComponent = component.lock())
            lockedComponent->SetOwner(static_pointer_cast<Actor>(this->shared_from_this()));
    }

    void Actor::RemoveComponent(weak_ptr<ActorComponent> component)
    {
        for (size_t i = 0; i < this->components.size(); i++)
        {
            if (this->components[i].lock() == component.lock())
            {
                this->components.erase(this->components.begin() + i);
                this->sceneContext->DestroyObject(component);
                return;
            }
        }
    }

    void Actor::toString(ostream &os) const
    {
        GameObject::toString(os);
        Vector3i pos = this->GetWorldPosition().cast<int>();
        Vector3i rot = this->GetWorldRotation().cast<int>();
        Vector3i scale = this->GetWorldScale().cast<int>();
        auto printV3 = [](Vector3i v3, ostream &os)
        { os << C_BLUE << v3.x() << C_RESET << "," << C_BLUE << v3.y() << C_RESET << "," << C_BLUE << v3.z() << C_RESET; };
        os << endl;
        os << "\t├──Pos(";
        printV3(pos, os);
        os << "), " << "Rot(";
        printV3(rot, os);
        os << "), " << "Scale(";
        printV3(scale, os);
        os << ")";
        if (this->components.size() != 0)
        {
            os << endl
               << "\t├──" << C_GREEN << "Components:" << C_CYAN << this->components.size() << C_RESET;
        }
        for (auto component : this->components)
        {
            if (auto lockedComponent = component.lock())
            {
                os << endl
                   << "\t│\t" << C_GREEN << "├─" << C_RESET;
                lockedComponent->toString(os);
            }
        }
    }
}