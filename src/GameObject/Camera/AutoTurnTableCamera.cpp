#include "AutoTurnTableCamera.hpp"

namespace REngine
{
    AutoTurnTableCamera::AutoTurnTableCamera(/* args */) {}
    AutoTurnTableCamera::AutoTurnTableCamera(json args) {}
    AutoTurnTableCamera::~AutoTurnTableCamera() {}

    void AutoTurnTableCamera::Tick(float deltatime)
    {
        rotaitonOffset += Vector3f(0, deltatime * 10, 0);
    };

    void AutoTurnTableCamera::SetLocalRotation(Vector3f rotation)
    {
        TurnTableCamera::SetLocalRotation(rotation + rotaitonOffset);
    }
}