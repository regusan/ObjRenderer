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
    void GameObject::BeginPlay()
    {
    }
    void GameObject::OnDestroyed()
    {
    }

    string GameObject::GetObjectName()
    {
        return this->name;
    }

    void GameObject::toString(ostream &os) const
    {
        os << C_RED << name << C_RESET << "(" << C_BLUE << shared_from_this() << C_RESET << ")";
    }
    ostream &GameObject::logStart(ostream &in) const
    {
        in << C_BLUE << this->name << C_RESET << endl;
        return in;
    }
}