#pragma once
#include "GameObject.hpp"
#include "../header/EigenHeader.hpp"
#include "../header/MathHeader.hpp"
using namespace Transform;

/// @brief シーン上に配置されるオブジェクト
class Actor : public GameObject
{
protected:
    Matrix4f mat = Matrix4f::Identity();
    virtual void matUpdate();

public:
    Vector3f location = Vector3f(0, 0, 0);
    Vector3f rotation = Vector3f(0, 0, 0);
    Vector3f scale = Vector3f(1, 1, 1);
    virtual void SetRotation(Vector3f rotate);
    virtual void SetPosition(Vector3f position);
    virtual void SetScale(Vector3f scale);
    Matrix4f getMat();
    Actor(/* args */);
    Actor(json args);
    ~Actor();
};
