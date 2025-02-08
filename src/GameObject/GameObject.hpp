#pragma once
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include "GameObjectFactory.hpp"
using namespace std;

/// @brief エンジン上で管理されるオブジェクト
class GameObject
{
private:
public:
    string name = "NULL";
    long uuid = 0;
    GameObject(/* args */);
    virtual ~GameObject(); // Virtualにして動的型解析有効化
    void Tick(float deltatime);
    void BeginPlay();
    string GetObjectName();
    friend ostream &operator<<(ostream &os, const GameObject &go)
    {
        os << "GameObject:" << go.name << "(" << go.uuid << ")" << endl;
        return os;
    }
};
// 文字列生成などの初期化のために使用
GCLASS_NO_ARG(GameObject)
