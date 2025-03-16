#pragma once

#include "Camera.hpp"

using namespace Transform;
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