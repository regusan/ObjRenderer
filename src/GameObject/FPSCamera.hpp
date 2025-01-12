#pragma once

#include "Camera.hpp"

using namespace Transform;
class FPSCamera : public Camera
{

public:
    FPSCamera(/* args */);
    ~FPSCamera();
    void OnUpdateInput(const XEvent &event) override;
    void matUpdate() override;
};
