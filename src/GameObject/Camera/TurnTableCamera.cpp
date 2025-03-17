#include "TurnTableCamera.hpp"

namespace REngine
{
    using namespace REngine::Input;
    TurnTableCamera::TurnTableCamera(/* args */) : FPSCamera()
    {
    }
    TurnTableCamera::TurnTableCamera(json args) : FPSCamera(args)
    {
    }

    TurnTableCamera::~TurnTableCamera()
    {
    }
    void TurnTableCamera::matUpdate()
    {
        this->mat = MakeMatOffset(Vector3f(0, 0, this->radius)) * MakeRotMatX(this->rotation.z()) * MakeRotMatY(this->rotation.y()) * MakeMatOffset(this->location);
    }
    void TurnTableCamera::Tick(float deltatime)
    {
        FPSCamera::Tick(deltatime);

        if (InputSubSystem::getInstance().GetKeyStatus(KeyID::Up).isHeld)
        {
            this->SetRadius(this->radius * 0.9);
            std::cout << "CameraBoolRadius:" << this->radius << std::endl;
        }

        if (InputSubSystem::getInstance().GetKeyStatus(KeyID::Down).isHeld)
        {
            this->SetRadius(this->radius * 1.1);
            std::cout << "CameraBoolRadius:" << this->radius << std::endl;
        }
    }
    void TurnTableCamera::SetRadius(float radius)
    {
        this->radius = radius;
    }
}