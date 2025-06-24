
// RaceCarController.hpp
#pragma once

#include "../../GameObject/Actor.hpp"
#include "../../GameObject/Camera/Camera.hpp"
#include "RaceCar.hpp"

namespace REngine::RaceGame
{
    /// @brief 車の操作を管理するコントローラー
    class RaceCarController : public Actor
    {
    protected:
        weak_ptr<RaceCar> controlledCar;
        weak_ptr<Camera> camera;
        weak_ptr<Actor> cameraArm;
        Vector3f cameraRotationOffset = Vector3f::Zero();

    public:
        RaceCarController(json args);
        RaceCarController();
        ~RaceCarController();

        virtual void BeginPlay() override;
        virtual void Tick(float deltaTime) override;
        virtual void OnDestroyed() override;

        void SetControlledCar(weak_ptr<RaceCar> car) { controlledCar = car; }
    };
    GCLASS(RaceCarController)
}