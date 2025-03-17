#include "GameObject.hpp"
#include "../GameObject/Scene.hpp"
namespace REngine
{
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
    void GameObject::OnDestroyed()
    {
        for (auto child : this->children)
            this->sceneContext->DestroyObject(child);
    }

    void GameObject::SetParent(weak_ptr<GameObject> parent)
    {
        this->parent = parent;
        // this->matUpdate();
    }
    void GameObject::DettachParent()
    {
        this->parent.reset();
        // this->matUpdate();
    }
    void GameObject::AddChild(weak_ptr<GameObject> child)
    {
        this->children.push_back(child);
        if (auto locked = child.lock())
            locked->SetParent(this->shared_from_this());
    }
    void GameObject::DettachChild(weak_ptr<GameObject> child)
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

    string GameObject::GetObjectName()
    {
        return this->name;
    }
}