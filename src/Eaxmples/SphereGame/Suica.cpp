
#include "../../GameObject/Scene.hpp"
#include "Suica.hpp"
namespace REngine::SphereGame
{
    using namespace REngine;
    Suica::Suica() : Actor()
    {
    }
    Suica::Suica(json args) : Suica()
    {
    }

    Suica::~Suica()
    {
    }
    void Suica::PostInitProperties()
    {
        // メッシュの準備
        this->suicaMesh = this->sceneContext->SpawnActorOfClass<MeshActor>();
        this->AddChild(this->suicaMesh);
        if (auto sp = suicaMesh.lock())
            sp->meshModel = REngine::AssetSubSystem::getInstance().meshManager.LoadAsset("GameAsset/SphereGame/SphereGameSphere.obj");

        // 物理系の設定
        this->rigitBody = this->sceneContext->SpawnActorOfClass<Component::RigitBody>();
        this->AddComponent(this->rigitBody);
        this->rigitBody.lock()->positionConstraint = Vector3i(0, 0, 1);

        this->collider = this->sceneContext->SpawnActorOfClass<Component::SphereCollider>();
        this->AddComponent(collider);
        this->collider.lock()->physicMaterial = PhysicMaterialExample::Concrete;

        this->spawnedTime = this->sceneContext->timeManager.GetCurrentTime();

        // スポーン直後は待機モードで物理無し
        this->collider.lock()->SetActive(false);
        this->rigitBody.lock()->SetActive(false);

        this->SetLevel(0);
    }
    void Suica::BeginPlay()
    {
        Actor::BeginPlay();
    }

    void Suica::Tick(float deltaTime)
    {
        Actor::Tick(deltaTime);

        // 他のスイカと衝突しているか調べる
        auto sp_collider = this->collider.lock();
        if (!sp_collider)
            return;
        if (this->HasDestroyMarker())
            return;
        vector<REngine::Hit> hits = sp_collider->GetAllOverlappingActor();
        for (auto &hit : hits)
        {
            auto sp = static_pointer_cast<Suica>(hit.hitActor.lock());
            if (!sp) // 衝突相手がスイカじゃなかったら無視
                continue;
            if (sp->level != this->level) // レベルが違うスイカだったら無視
                continue;
            if (sp->HasDestroyMarker()) // 破棄が進行中のオブジェクトなら無視
                continue;

            // 古いほうを遺す
            shared_ptr<SphereGame::Suica> live;
            shared_ptr<SphereGame::Suica> dead;
            if (sp->spawnedTime > this->spawnedTime)
            {
                live = sp;
                dead = static_pointer_cast<SphereGame::Suica>(this->shared_from_this());
            }
            else
            {
                live = static_pointer_cast<SphereGame::Suica>(this->shared_from_this());
                dead = sp;
            }
            live->SetLevel(this->level + 1);
            this->sceneContext->DestroyObject(dead);
        }
    }
    void Suica::OnDestroyed()
    {
        Actor::OnDestroyed();
    }

    void Suica::SetLevel(int newLevel)
    {
        static vector<string> modelpathes = {
            "GameAsset/SphereGame/SphereGameSphere0.obj",
            "GameAsset/SphereGame/SphereGameSphere1.obj",
            "GameAsset/SphereGame/SphereGameSphere2.obj",
        };
        this->radius = this->initialRadius * powf(this->scaleUpRate, newLevel);
        if (auto unlocked = this->suicaMesh.lock())
        {
            unlocked->SetLocalScale(Vector3f(this->radius, this->radius, this->radius));
        }
        if (auto unlocked = this->collider.lock())
        {
            unlocked->radius = this->radius * 1.3; // 少し大きなコリジョンにする
        }
        // レベルに対応したメッシュのロード
        if (auto sp = suicaMesh.lock())
            sp->meshModel = REngine::AssetSubSystem::getInstance().meshManager.LoadAsset(modelpathes[newLevel % modelpathes.size()]);

        this->level = newLevel;
    }
    void Suica::Drop()
    {

        this->DettachParent();
        collider.lock()->SetActive(true);
        this->rigitBody.lock()->SetActive(true);
    }

}