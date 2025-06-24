// RaceCar.cpp
#include "../../GameObject/Scene.hpp"
#include "RaceCar.hpp"
#include "RaceGameCollisionLayer.hpp"

namespace REngine::RaceGame
{
    using namespace REngine;

    RaceCar::RaceCar() : Actor()
    {
    }

    RaceCar::RaceCar(json args) : RaceCar()
    {
    }

    RaceCar::~RaceCar()
    {
    }

    weak_ptr<MeshActor> RaceCar::SpawnMeshActor(const string &path)
    {
        auto mesh = this->sceneContext->SpawnActorOfClass<MeshActor>();
        this->AddChild(mesh);
        if (auto sp = mesh.lock())
            sp->meshModel = REngine::AssetSubSystem::getInstance().meshManager.LoadAsset(path);
        return mesh;
    }
    void RaceCar::PostInitProperties()
    {
        // 車のメッシュの準備
        this->carMesh = this->SpawnMeshActor("GameAsset/Common/City/car_hatchback.obj");
        this->leftRearWheelMesh = this->SpawnMeshActor("GameAsset/Common/City/car_hatchback_wheel_rear_left.obj");
        this->rightRearWheelMesh = this->SpawnMeshActor("GameAsset/Common/City/car_hatchback_wheel_rear_right.obj");
        this->leftFrontWheelMesh = this->SpawnMeshActor("GameAsset/Common/City/car_hatchback_wheel_front_left.obj");
        this->rightFrontWheelMesh = this->SpawnMeshActor("GameAsset/Common/City/car_hatchback_wheel_front_right.obj");

        this->light = this->sceneContext->SpawnActorOfClass<SpotLightActor>();
        this->AddChild(light);
        if (auto sp = light.lock())
        {
            sp->SetLocalRotation(Vector3f(0, 0, 0));
            sp->SetLocalPosition(Vector3f(0, 2, 0));
            sp->length = 1000;
        }

        // 物理系の設定
        this->rigidBody = this->sceneContext->SpawnActorOfClass<Component::RigitBody>();
        this->AddComponent(this->rigidBody);
        if (auto rb = rigidBody.lock())
        {
            // rb->mass = this->mass;
            rb->linearDamping = 0.1f;
            rb->angularDamping = 0.99f;
            rb->positionConstraint = Vector3i(0, 1, 0); // Y軸方向の動きを制限（地面に沿って移動）
        }

        // コライダーの設定
        this->collider = this->sceneContext->SpawnActorOfClass<Component::SphereCollider>();
        this->AddComponent(collider);
        if (auto col = collider.lock())
        {
            // col->size = Vector3f(2.0f, 1.0f, 4.5f); // 車の大きさに合わせて調整
            col->physicMaterial = PhysicMaterialExample::Metal;
        }
    }

    void RaceCar::BeginPlay()
    {
        Actor::BeginPlay();
    }

    void RaceCar::Tick(float deltaTime)
    {
        Actor::Tick(deltaTime);

        auto rb = rigidBody.lock();
        if (!rb)
            return;

        // 車の前方向ベクトルを取得
        Vector3f forward = Transform::GetForwardVector(this->getWorldMat());
        Vector3f right = Transform::GetRightVector(this->getWorldMat());

        // 現在速度の計算 (前方向の速度成分のみを考慮)
        Vector3f velocity = rb->velocity;
        currentSpeed = velocity.dot(forward);

        // アクセル操作による前進力
        float accelerationMultiplier = (maxSpeed - abs(currentSpeed)) / maxSpeed; // 最高速度に近づくと加速力が減少
        if (accelerationMultiplier < 0)
            accelerationMultiplier = 0;

        Vector3f accelerationForceVector = forward * throttleInput * accelerationForce * accelerationMultiplier;

        // ブレーキ力 (現在の速度と逆方向に作用)
        Vector3f brakeForceVector = -velocity.normalized() * brakeInput * brakeForce - forward * accelerationForce * accelerationMultiplier * brakeInput;

        // 空気抵抗 (速度の二乗に比例)
        Vector3f dragForceVector = -velocity * velocity.norm() * dragFactor;

        // 合力の適用
        rb->AddForce(accelerationForceVector + brakeForceVector + dragForceVector);

        // ステアリング処理
        // 速度に応じてステアリング効果を調整（低速時の方が曲がりやすい）
        float speedFactor = clamp<float>(abs(currentSpeed) / 10.0f, 0.1f, 1.0f);
        float steeringAmount = steeringInput * steeringForce * speedFactor;

        // 速度がある場合のみステアリングを適用
        if (abs(currentSpeed) > 0.5f)
        {
            // 上方向を中心にしたトルクを適用して車を回転
            rb->AddTorque(Vector3f::UnitY() * steeringAmount * (currentSpeed > 0 ? 1.0f : -1.0f));

            // 現在のステアリング角度を更新
            currentSteeringAngle = steeringInput * maxSteeringAngle;
        }
        else
        {
            currentSteeringAngle = 0.0f;
        }

        // タイヤの視覚的な回転
        if (auto wheel = leftFrontWheelMesh.lock())
        {
            wheel->SetLocalRotation(Vector3f(0, currentSteeringAngle, 0));
        }
        if (auto wheel = rightFrontWheelMesh.lock())
        {
            wheel->SetLocalRotation(Vector3f(0, clamp<float>(currentSteeringAngle, -30, 30), 0));
        }
    }

    void RaceCar::OnDestroyed()
    {
        Actor::OnDestroyed();
    }

    void RaceCar::ApplyThrottle(float value)
    {
        throttleInput = clamp<float>(value, 0.0f, 1.0f);
    }

    void RaceCar::ApplyBrake(float value)
    {
        brakeInput = clamp<float>(value, 0.0f, 1.0f);
    }

    void RaceCar::ApplySteering(float value)
    {
        steeringInput = clamp<float>(value, -1.0f, 1.0f);
    }
}