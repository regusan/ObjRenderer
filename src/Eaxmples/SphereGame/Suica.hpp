#pragma once

#include "../../GameObject/Actor.hpp"
#include "../../GameObject/Mesh.hpp"
#include "../../GameObject/Component/RigitBody.hpp"
#include "../../GameObject/Component/Physics/SphereCollider.hpp"
#include <future>
#include <chrono>
#include <vector>
namespace REngine::SphereGame
{
    /// @brief スイカのアクター
    class Suica : public Actor
    {
    protected:
        float initialRadius = 0.6f;
        float scaleUpRate = 1.40f; // レベルアップ時の拡大率

        float radius = 0.15f;
        int level = 0;
        float spawnedTime; // 生成された時間
        weak_ptr<MeshActor> suicaMesh;
        weak_ptr<Component::SphereCollider> collider;
        weak_ptr<Component::RigitBody> rigitBody;

    public:
        Suica(json args);
        Suica();
        ~Suica();
        virtual void PostInitProperties() override;
        virtual void BeginPlay() override;
        virtual void Tick(float deltaTime) override;
        virtual void OnDestroyed() override;

        void SetLevel(int newLevel); // 指定レベルへ
        void Drop();                 // 投下モードにする
    };
    GCLASS(Suica)
}
