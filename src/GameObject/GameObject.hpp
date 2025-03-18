#pragma once
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "GameObjectFactory.hpp"
#include "../Engine/Input/InputEvent.hpp"
#include "../Engine/Input/InputSubSystem.hpp"

using json = nlohmann::json;
namespace REngine
{
    using namespace std;
    using namespace REngine::Terminal;
    using namespace REngine::Input;
    // 前方宣言
    class Scene;
    /// @brief エンジン上で管理されるオブジェクト
    class GameObject : public enable_shared_from_this<GameObject>
    {
    protected:
    public:
        Scene *sceneContext = nullptr;
        string name = "NULL";
        long uuid = 0;

        weak_ptr<GameObject> parent;
        vector<weak_ptr<GameObject>> children;

        GameObject(/* args */);
        virtual ~GameObject(); // Virtualにして動的型解析有効化
        virtual void Tick(float deltatime);
        virtual void BeginPlay();
        virtual void OnDestroyed();

        // 親子系
        virtual void SetParent(weak_ptr<GameObject> parent);
        virtual void DettachParent();
        virtual void AddChild(weak_ptr<GameObject> child);
        virtual void DettachChild(weak_ptr<GameObject> child);

        string GetObjectName();
        void SetSpawnedScene(Scene *_scene);

        virtual void toString(ostream &os) const;

        friend ostream &operator<<(ostream &os, const GameObject &go)
        {
            go.toString(os);
            return os;
        }
    };
    // 文字列生成などの初期化のために使用
    GCLASS_NO_ARG(GameObject)
}