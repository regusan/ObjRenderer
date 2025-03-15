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
#include "../Engine/FileWatcher.hpp"

using namespace std;

/// @brief シーン管理クラス
class Scene
{
protected:
    vector<shared_ptr<GameObject>> objects;
    FileWatcher fileWatcher;

public:
    template <typename T, typename... Args>
    shared_ptr<T> SpawnActorOfClass(Args &&...args)
    {
        // GameObject継承出ないものをスポーンしていたらあさーと
        static_assert(is_base_of<GameObject, T>::value, "GameObject継承ではないクラスはスポーンできません。");

        shared_ptr<T> obj = make_shared<T>(forward<Args>(args)...);
        obj->SetSpawnedScene(this);
        objects.push_back(move(obj));
        return obj;
    }

    /// @brief Jsonからシーンを構築
    /// @param sceneJson
    void loadScene(json sceneJson);

    void loadScene(filesystem::path scenepath);

    void OnFileChanged(const filesystem::path &);

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

    void ExecTick(const float deltatime);
    void ExecBeginPlay();

    friend ostream &operator<<(ostream &os, const Scene &go)
    {
        for (const auto &obj : go.objects)
            os << obj.get();
        return os;
    }
};