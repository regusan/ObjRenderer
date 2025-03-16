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
using namespace std;

// 前方宣言
class Scene;

/// @brief エンジン上で管理されるオブジェクト
class GameObject : public std::enable_shared_from_this<GameObject>
{
protected:
public:
    Scene *sceneContext = nullptr;
    string name = "NULL";
    long uuid = 0;
    GameObject(/* args */);
    virtual ~GameObject(); // Virtualにして動的型解析有効化
    virtual void Tick(float deltatime);
    virtual void BeginPlay();
    virtual void OnDestroyed();
    string GetObjectName();
    void SetSpawnedScene(Scene *_scene);
    friend ostream &operator<<(ostream &os, const GameObject &go)
    {
        os << "GameObject:" << go.name << "(" << go.uuid << ")" << endl;
        return os;
    }
};
// 文字列生成などの初期化のために使用
GCLASS_NO_ARG(GameObject)
