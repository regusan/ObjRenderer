
#include "../../GameObject/Scene.hpp"
#include "Suica.hpp"
#include "DropPlayer.hpp"
namespace REngine::SphereGame
{
    using namespace REngine;
    DropPlayer::DropPlayer() : Actor()
    {
    }
    DropPlayer::DropPlayer(json args) : DropPlayer()
    {
    }

    DropPlayer::~DropPlayer()
    {
    }
    void DropPlayer::BeginPlay()
    {
        Actor::BeginPlay();

        this->cursorMesh = this->sceneContext->SpawnActorOfClass<MeshActor>();
        this->AddChild(this->cursorMesh);
        if (auto sp = cursorMesh.lock())
        {
            sp->meshModel = REngine::AssetSubSystem::getInstance().meshManager.LoadAsset("GameAsset/SphereGame/TriangleArrow.obj");
            sp->SetLocalScale(Vector3f(0.4, 0.4, 0.4));
            sp->SetLocalPosition(Vector3f(0, 2, 0));
        }
        this->initialPosition = this->GetWorldPosition();

        this->coolTimeTimer = this->sceneContext->timeManager.CreateTimer(2, false, [this]()
                                                                          { this->SpawnNewSuica(this->GetLevel2Spawn()); }, false);
        this->SpawnNewSuica(0);
    }

    void DropPlayer::Tick(float deltaTime)
    {
        Actor::Tick(deltaTime);
        InputSubSystem &input = InputSubSystem::getInstance();

        // 移動処理
        if (input.GetKeyStatus(KeyID::A).isHeld || input.GetKeyStatus(KeyID::Left).isHeld)
            xMoveAmount += this->cursorSpeed * deltaTime;
        if (input.GetKeyStatus(KeyID::D).isHeld || input.GetKeyStatus(KeyID::Right).isHeld)
            xMoveAmount -= this->cursorSpeed * deltaTime;

        this->xMoveAmount = clamp<float>(xMoveAmount, this->moveRange.x(), this->moveRange.y());
        this->SetLocalPosition(this->initialPosition + Vector3f(xMoveAmount, 0, 0));

        // スイカの投下処理
        if (input.GetKeyStatus(KeyID::F).isPressed || input.GetKeyStatus(KeyID::MouseRight).isPressed)
        {
            if (auto sp = this->holdingSuica.lock())
            {
                sp->Drop();
                this->holdingSuica.lock()->SetLocalPosition(this->GetLocalPosition());
                this->holdingSuica.reset();
                this->coolTimeTimer->Start();
            }
        }

        // 矢印を回転
        if (auto sp = cursorMesh.lock())
        {
            sp->SetLocalRotation(sp->rotation + Vector3f::UnitY() * deltaTime * 100);
        }
    }
    void DropPlayer::OnDestroyed()
    {
        Actor::OnDestroyed();
    }

    weak_ptr<Suica> DropPlayer::SpawnNewSuica(int level)
    {
        this->holdingSuica = this->sceneContext->SpawnActorOfClass<Suica>();
        this->holdingSuica.lock()->SetLevel(this->GetLevel2Spawn());
        this->AddChild(this->holdingSuica);
        this->dropCount++;
        return this->holdingSuica;
    }

    int DropPlayer::GetLevel2Spawn()
    {
        int maxSpawnLevel = 2;
        if (this->dropCount > 5)
            maxSpawnLevel = 3;
        if (this->dropCount > 10)
            maxSpawnLevel = 4;

        return random() % maxSpawnLevel;
    }
}