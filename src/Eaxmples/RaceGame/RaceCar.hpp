// RaceCar.hpp
#pragma once

#include "../../GameObject/Actor.hpp"
#include "../../GameObject/Mesh.hpp"
#include "../../GameObject/Component/RigitBody.hpp"
#include "../../GameObject/Component/Physics/SphereCollider.hpp"
#include "../../GameObject/Light/SpotLightActor.hpp"
#include <future>
#include <chrono>
#include <vector>

namespace REngine::RaceGame
{
    /// @brief レース用の車のアクター
    class RaceCar : public Actor
    {
    protected:
        // 車の物理特性
        float maxSpeed = 5.0f;           // 最大速度
        float accelerationForce = 0.50f; // 加速力
        float brakeForce = 1.0f;         // ブレーキ力
        float steeringForce = 100.0f;    // ステアリング力
        float dragFactor = 0.05f;        // 空気抵抗係数
        float mass = 1.0f;               // 車の質量(kg)

        // 車の状態
        float currentSpeed = 0.0f;         // 現在の速度
        float currentSteeringAngle = 0.0f; // 現在のステアリング角度
        float maxSteeringAngle = 35.0f;    // 最大ステアリング角度

        // コンポーネント参照
        weak_ptr<MeshActor> carMesh;
        weak_ptr<MeshActor> rightFrontWheelMesh;
        weak_ptr<MeshActor> leftFrontWheelMesh;
        weak_ptr<MeshActor> rightRearWheelMesh;
        weak_ptr<MeshActor> leftRearWheelMesh;
        weak_ptr<Component::SphereCollider> collider;
        weak_ptr<Component::RigitBody> rigidBody;

        weak_ptr<SpotLightActor> light;

        // 入力値
        float throttleInput = 0.0f; // アクセル入力 (0.0 ~ 1.0)
        float brakeInput = 0.0f;    // ブレーキ入力 (0.0 ~ 1.0)
        float steeringInput = 0.0f; // ステアリング入力 (-1.0 ~ 1.0)

        weak_ptr<MeshActor> SpawnMeshActor(const string &path);

    public:
        RaceCar(json args);
        RaceCar();
        ~RaceCar();

        virtual void PostInitProperties() override;
        virtual void BeginPlay() override;
        virtual void Tick(float deltaTime) override;
        virtual void OnDestroyed() override;

        // 操作メソッド
        void ApplyThrottle(float value); // アクセル操作 (0.0 ~ 1.0)
        void ApplyBrake(float value);    // ブレーキ操作 (0.0 ~ 1.0)
        void ApplySteering(float value); // ステアリング操作 (-1.0 ~ 1.0)

        // 車両情報取得メソッド
        float GetCurrentSpeed() const { return currentSpeed; }
        float GetSteeringAngle() const { return currentSteeringAngle; }

        // 設定メソッド
        void SetMaxSpeed(float speed) { maxSpeed = speed; }
        void SetAccelerationForce(float force) { accelerationForce = force; }
        void SetBrakeForce(float force) { brakeForce = force; }
        void SetSteeringForce(float force) { steeringForce = force; }
    };
    GCLASS(RaceCar)
}