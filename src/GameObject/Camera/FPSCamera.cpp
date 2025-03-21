#include "FPSCamera.hpp"
using namespace REngine::Input;
FPSCamera::FPSCamera(/* args */)
{
}
FPSCamera::FPSCamera(json args) : Camera(args)
{
}

FPSCamera::~FPSCamera()
{
}

void FPSCamera::matUpdate()
{
    Actor::matUpdate();
    return;
}

void FPSCamera::Tick(float deltatime)
{
    InputSubSystem &input = InputSubSystem::getInstance();
    this->matUpdate();
    if (input.GetKeyStatus(KeyID::W).isHeld)
        SetLocalPosition(this->location + GetForwardVector(this->worldMatrix) * this->speed * deltatime);
    if (input.GetKeyStatus(KeyID::S).isHeld)
        SetLocalPosition(this->location - GetForwardVector(this->worldMatrix) * this->speed * deltatime);
    if (input.GetKeyStatus(KeyID::A).isHeld)
        SetLocalPosition(this->location - GetRightVector(this->worldMatrix) * this->speed * deltatime);
    if (input.GetKeyStatus(KeyID::D).isHeld)
        SetLocalPosition(this->location + GetRightVector(this->worldMatrix) * this->speed * deltatime);
    if (input.GetKeyStatus(KeyID::E).isHeld)
        SetLocalPosition(this->location - GetUpVector(this->worldMatrix) * this->speed * deltatime);
    if (input.GetKeyStatus(KeyID::Q).isHeld)
        SetLocalPosition(this->location + GetUpVector(this->worldMatrix) * this->speed * deltatime);

    this->SetLocalRotation(Vector3f(-input.axisState.position.y(), -input.axisState.position.x(), 0));
}
