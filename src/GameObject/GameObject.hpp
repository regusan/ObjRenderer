#pragma once
#include <typeinfo>
#include <string>

using namespace std;

/// @brief エンジン上で管理されるオブジェクト
class GameObject
{
private:
public:
    GameObject(/* args */);
    ~GameObject();
    void Tick(float deltatime);
    void BeginPlay();
    string GetObjectName();
};
