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
    this->mat = MakeRotMatX(this->rotation.z()) * MakeRotMatY(this->rotation.y()) * MakeMatOffset(this->location);
}

void FPSCamera::Tick(float deltatime)
{
    InputSubSystem &input = InputSubSystem::getInstance();
    if (input.GetKeyStatus(KeyID::W).isHeld)
        SetPosition(this->location - GetForwardVector(this->mat.inverse()).head<3>() * this->speed * deltatime);
    if (input.GetKeyStatus(KeyID::S).isHeld)
        SetPosition(this->location + GetForwardVector(this->mat.inverse()).head<3>() * this->speed * deltatime);
    if (input.GetKeyStatus(KeyID::A).isHeld)
        SetPosition(this->location + GetRightVector(this->mat.inverse()).head<3>() * this->speed * deltatime);
    if (input.GetKeyStatus(KeyID::D).isHeld)
        SetPosition(this->location - GetRightVector(this->mat.inverse()).head<3>() * this->speed * deltatime);
    if (input.GetKeyStatus(KeyID::E).isHeld)
        SetPosition(this->location + GetUpVector(this->mat.inverse()).head<3>() * this->speed * deltatime);
    if (input.GetKeyStatus(KeyID::Q).isHeld)
        SetPosition(this->location - GetUpVector(this->mat.inverse()).head<3>() * this->speed * deltatime);

    this->SetRotation(Vector3f(0, input.axisState.position.x(), input.axisState.position.y()));
}
