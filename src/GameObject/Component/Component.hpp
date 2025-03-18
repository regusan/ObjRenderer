#pragma once

#include "../GameObject.hpp"
namespace REngine::Component
{
    /// @brief メッシュを持つアクター
    class Component : public GameObject
    {
    private:
    public:
        weak_ptr<GameObject> owner;
        bool isActive;
        Component();
        Component(json args);
        ~Component();
    };
    GCLASS(Component)
}