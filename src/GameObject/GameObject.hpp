#pragma once

class GameObject
{
private:
public:
    GameObject(/* args */);
    ~GameObject();
    void Tick(float deltatime);
    void BeginPlay();
};
