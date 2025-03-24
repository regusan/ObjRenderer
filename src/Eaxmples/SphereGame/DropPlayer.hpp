#pragma once

#include "../../GameObject/Actor.hpp"
#include "../../GameObject/Mesh.hpp"
#include "../../GameObject/Camera/Camera.hpp"
#include "../../GameObject/Component/RigitBody.hpp"
#include "../../GameObject/Component/Physics/SphereCollider.hpp"
#include <future>
#include <chrono>
namespace REngine::SphereGame
{
    class Suica; // 先方宣言
    /// @brief スイカを落とすプレイヤー
    class DropPlayer : public Actor
    {
    protected:
        Vector3f initialPosition; // 移動の原点となる位置
        float xMoveAmount = 0;    // 原点からどれだけ移動したか

        weak_ptr<MeshActor> cursorMesh;            // カーソルのメッシュ子供アクタ
        weak_ptr<Suica> holdingSuica;              // 今持ってるスイカ
        shared_ptr<Timer> coolTimeTimer = nullptr; // 連続投下を防止するタイマー

        int dropCount = 0; // 投下数をカウントし進捗とする

        int GetLevel2Spawn(); // ランダムなレベルを取得。進捗によって変化する。

    public:
        float cursorSpeed = 5.0f;
        Vector2f moveRange = Vector2f(-4.5f, 4.5f); // 移動可能な範囲

        DropPlayer(json args);
        DropPlayer();
        ~DropPlayer();

        virtual void BeginPlay() override;
        virtual void Tick(float deltaTime) override;
        virtual void OnDestroyed() override;

        weak_ptr<Suica> SpawnNewSuica(int level); // 指定レベルでスイカをスポーン
    };
    GCLASS(DropPlayer)
}
