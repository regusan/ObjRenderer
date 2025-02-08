#pragma once
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

#include "GameObject.hpp"
#include "GameObjectFactory.hpp"

using namespace std;

/// @brief シーン管理クラス
class Scene
{
protected:
    vector<shared_ptr<GameObject>> objects;

public:
    /// @brief Jsonからシーンを構築
    /// @param sceneJson
    void loadFromJson(json sceneJson)
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
                objects.push_back(move(obj));
            }
        }
    }

    /// @brief シーン上の指定クラスの全てのオブジェクトを取得
    /// @tparam T
    /// @return
    template <typename T>
    vector<shared_ptr<T>> GetObjectsOfClass()
    {
        vector<shared_ptr<T>> retval;
        for (auto &obj : this->objects)
        {
            if (obj)
            {
                shared_ptr<T> casted = dynamic_pointer_cast<T>(obj);
                if (casted)
                    retval.push_back(casted);
            }
        }
        return retval;
    }

    void ExecTick(const float deltatime)
    {
        for (auto &obj : this->objects)
        {
            if (obj)
                obj->Tick(deltatime);
        }
    }

    friend ostream &operator<<(ostream &os, const Scene &go)
    {
        for (const auto &obj : go.objects)
            os << obj.get();
        return os;
    }
};