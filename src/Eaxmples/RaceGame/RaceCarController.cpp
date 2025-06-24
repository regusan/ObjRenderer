// RaceCarController.cpp
#include "../../GameObject/Scene.hpp"
#include "RaceCarController.hpp"
#include "RaceCar.hpp"

namespace REngine::RaceGame
{
    using namespace REngine;

    RaceCarController::RaceCarController() : Actor()
    {
    }

    RaceCarController::RaceCarController(json args) : Actor(args)
    {
    }

    RaceCarController::~RaceCarController()
    {
    }

    void RaceCarController::BeginPlay()
    {
        Actor::BeginPlay();
        if (this->camera.expired())
        {
            auto newCamera = this->sceneContext->SpawnActorOfClass<Camera>();
            this->camera = newCamera;

            if (auto sp = this->camera.lock())
            {
                sp->SetLocalPosition(Vector3f(0, 2, -2));
            }
        }
        // 車が設定されていない場合は新しく生成
        if (controlledCar.expired())
        {
            auto newCar = this->sceneContext->SpawnActorOfClass<RaceCar>();
            controlledCar = newCar;
            if (auto sp = controlledCar.lock())
            {
                sp->AddChild(this->camera);
            }
            this->AddChild(controlledCar);
        }
    }

    void RaceCarController::Tick(float deltaTime)
    {
        Actor::Tick(deltaTime);

        auto car = controlledCar.lock();
        if (!car)
            return;

        InputSubSystem &input = InputSubSystem::getInstance();

        // アクセル操作（W/Up キー）
        float throttleValue = 0.0f;
        if (input.GetKeyStatus(KeyID::W).isHeld || input.GetKeyStatus(KeyID::Up).isHeld)
            throttleValue = 1.0f;
        car->ApplyThrottle(throttleValue);

        // ブレーキ操作（S/Down キー）
        float brakeValue = 0.0f;
        if (input.GetKeyStatus(KeyID::S).isHeld || input.GetKeyStatus(KeyID::Down).isHeld)
            brakeValue = 1.0f;
        car->ApplyBrake(brakeValue);

        // ステアリング操作（A/D または Left/Right キー）
        float steeringValue = 0.0f;
        if (input.GetKeyStatus(KeyID::A).isHeld || input.GetKeyStatus(KeyID::Left).isHeld)
        {
            steeringValue += 1.0f;
            cameraRotationOffset.y() += 1.0f;
        }
        if (input.GetKeyStatus(KeyID::D).isHeld || input.GetKeyStatus(KeyID::Right).isHeld)
        {
            steeringValue -= 1.0f;
            cameraRotationOffset.y() -= 1.0f;
        }

        cameraRotationOffset.y() += -cameraRotationOffset.y() * 0.05f;
        car->ApplySteering(steeringValue);

        if (auto sp = this->camera.lock())
        {
            sp->SetLocalRotation(Vector3f(-60, 0, 180) + cameraRotationOffset);
        }
    }

    void RaceCarController::OnDestroyed()
    {
        Actor::OnDestroyed();
    }
}