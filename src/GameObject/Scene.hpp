#pragma once
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <list>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

#include "../Engine/TermFormatter.hpp"
#include "../Engine/FileWatcher.hpp"
#include "../Engine/Time/TimeManager.hpp"

// Factory を最後にする
#include "GameObjectFactory.hpp"

namespace REngine
{
    using namespace std;

    // 先方宣言
    class GameObject;

    /// @brief シーン管理クラス
    class Scene
    {
    protected:
        list<shared_ptr<GameObject>> objects;  // 現在スポーンされているオブジェクト
        list<weak_ptr<GameObject>> newobjects; // 現在生成待機中のオブジェクト
        FileWatcher fileWatcher;               // ファイルの自動更新のためのウォッチャー

    public:
    public:
        REngine::TimeManager timeManager = REngine::TimeManager(); // 同期タイマーの生成破棄管理
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
            obj->uuid = rand();
            obj->SetSpawnedScene(this);
            this->newobjects.push_back(obj);
            objects.push_back(obj);
            count++;
            return obj;
        }
        /// @brief 指定のオブジェクトを破棄
        /// @param obj
        void DestroyObject(weak_ptr<GameObject> obj);

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
                if (auto casted = dynamic_pointer_cast<T>(obj))
                {
                    if (casted)
                    {
                        retval.emplace_back(casted);
                    }
                }
            }
            return retval;
        }

        float ExecTick();
        void ExecBeginPlay();

        stringstream hieralcyToString();

        friend ostream &operator<<(ostream &os, const Scene &go)
        {
            for (const auto &obj : go.objects)
                os << obj.get() << endl;
            return os;
        }
    };
}