#pragma once

#include "Camera.hpp"

namespace REngine
{
    class FPSCamera : public Camera
    {

    public:
        FPSCamera(/* args */);
        FPSCamera(json args);
        ~FPSCamera();
        void matUpdate() override;
        void Tick(float deltatime) override;
    };
    GCLASS(FPSCamera)
}