#include "AutoTurnTableCamera.hpp"

AutoTurnTableCamera::AutoTurnTableCamera(/* args */) {}
AutoTurnTableCamera::AutoTurnTableCamera(json args) {}
AutoTurnTableCamera::~AutoTurnTableCamera() {}

void AutoTurnTableCamera::Tick(float deltatime)
{
    rotaitonOffset += Vector3f(0, deltatime * 10, 0);
};

void AutoTurnTableCamera::SetRotation(Vector3f rotation)
{
    TurnTableCamera::SetRotation(rotation + rotaitonOffset);
}