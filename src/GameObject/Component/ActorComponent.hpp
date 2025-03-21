#pragma once

#include "../GameObject.hpp"
#include "../Actor.hpp"
namespace REngine
{
    class Actor; // 前方宣言
}
namespace REngine::Component
{
    /// @brief コンポーネントの基底クラス
    class ActorComponent : public GameObject
    {
    private:
        bool bIsActive = true;

    public:
        weak_ptr<Actor> owner;
        void SetOwner(const weak_ptr<Actor> owner);
        weak_ptr<Actor> GetOwner() const;
        void SetActive(const bool active);
        bool GetActive() const;

        ActorComponent();
        ActorComponent(json args);
        ~ActorComponent();
    };
    GCLASS(ActorComponent)
}