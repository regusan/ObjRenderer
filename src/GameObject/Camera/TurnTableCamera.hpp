#pragma once
#include "Camera.hpp"
#include "FPSCamera.hpp"

/// @brief ある点を中心に回転するカメラ
class TurnTableCamera : public FPSCamera
{
private:
    float radius = 10;
    void matUpdate() override;

public:
    TurnTableCamera(/* args */);
    TurnTableCamera(json args);
    ~TurnTableCamera();
    /// @brief カメラブームの半径
    /// @param radius
    void SetRadius(float radius);
    void OnUpdateInput(const XEvent &event) override;
};
GCLASS(TurnTableCamera)