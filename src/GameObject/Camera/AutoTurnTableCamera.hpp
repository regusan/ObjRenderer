#pragma once
#include "TurnTableCamera.hpp"

namespace REngine
{
    /// @brief ある点を中心に回転するカメラ
    class AutoTurnTableCamera : public TurnTableCamera
    {
    private:
        Vector3f rotaitonOffset = Vector3f::Zero();

    public:
        AutoTurnTableCamera(/* args */);
        AutoTurnTableCamera(json args);
        ~AutoTurnTableCamera();

        void Tick(float deltatime) override;
        void SetRotation(Vector3f rotation) override;
    };
    GCLASS(AutoTurnTableCamera)
}