#pragma once
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <list>
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
    list<shared_ptr<GameObject>> objects;
    FileWatcher fileWatcher;

public:
    template <typename T, typename... Args>
    enable_if_t<is_base_of<GameObject, T>::value, weak_ptr<T>>
    SpawnActorOfClass(Args &&...args)
    {
        static unsigned long long int count = 0;

        // GameObject継承出ないものをスポーンしていたらあさーと
        static_assert(is_base_of<GameObject, T>::value, "GameObject継承ではないクラスはスポーンできません。");

        shared_ptr<T> obj = make_shared<T>(forward<Args>(args)...);
        stringstream ss;
        ss << typeid(T).name() << "_" << count;
        obj->name = ss.str();
        obj->SetSpawnedScene(this);
        obj->BeginPlay();
        objects.push_back(obj);
        count++;
        return obj;
    }
    void DestroyActor(weak_ptr<GameObject> obj);

    /// @brief Jsonからシーンを構築
    /// @param sceneJson
    void loadScene(json sceneJson);

    void loadScene(filesystem::path scenepath);

    void OnFileChanged(const filesystem::path &);

    /// @brief シーン上の指定クラスの全てのオブジェクトを取得
    /// @tparam T
    /// @return
    template <typename T>
    vector<weak_ptr<T>> GetObjectsOfClass()
    {
        vector<weak_ptr<T>> retval;
        for (auto obj : this->objects)
        {
            if (obj)
            {
                auto casted = dynamic_pointer_cast<T>(obj);
                if (casted)
                    retval.emplace_back(casted);
            }
        }
        return retval;
    }

    void ExecTick(const float deltatime);
    void ExecBeginPlay();

    stringstream hieralcyToString();

    friend ostream &operator<<(ostream &os, const Scene &go)
    {
        for (const auto &obj : go.objects)
            os << obj.get() << endl;
        return os;
    }
};