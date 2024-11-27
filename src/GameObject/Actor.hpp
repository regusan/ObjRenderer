#pragma once
#include "GameObject.hpp"
#include "../header/EigenHeader.hpp"
class Actor : GameObject
{
private:
public:
    Vector3f location;
    Vector3f rotation;
    Actor(/* args */);
    ~Actor();
};
