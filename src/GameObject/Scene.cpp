#include "Scene.hpp"

void Scene::loadFromJson(json sceneJson)
{
    for (auto &[actorName, actorData] : sceneJson["GameObjects"].items())
    {

        string classType = actorData["class"];
        json args = actorData["args"];
        // cout << actorData << endl;

        // 指定型でオブジェクトを作成
        shared_ptr<GameObject> obj = GameObjectFactory::createObject(classType, actorName, args);
        if (obj)
        {
            obj->SetSpawnedScene(this);
            objects.push_back(move(obj));
        }
    }
}

void Scene::ExecTick(const float deltatime)
{
    for (auto &obj : this->objects)
    {
        if (obj)
            obj->Tick(deltatime);
    }
}
void Scene::ExecBeginPlay()
{
    for (auto &obj : this->objects)
    {
        if (obj)
            obj->BeginPlay();
    }
}
