#pragma once
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "GameObjectFactory.hpp"
#include "../Engine/Input/InputEvent.hpp"
#include "../Engine/Input/InputSubSystem.hpp"

using json = nlohmann::json;
namespace REngine::Component
{
    class ActorComponent;
}
namespace REngine
{
    using namespace std;
    using namespace REngine::Terminal;
    using namespace REngine::Input;
    // 前方宣言
    class Scene;
    class GameObject;
    /// @brief エンジン上で管理されるオブジェクト
    class GameObject : public enable_shared_from_this<GameObject>
    {
    protected:
        /// @brief オブジェクトの破棄が進行中かのフラグ
        bool hasDestroyMarker = false;

    public:
        /// @brief このオブジェクトの破棄が進行中かを示すフラグ
        Scene *sceneContext = nullptr;
        /// @brief このオブジェクトの名前
        string name = "NULL";
        /// @brief このオブジェクトのUUID
        long uuid = 0;

        GameObject(/* args */);
        virtual ~GameObject(); // Virtualにして動的型解析有効化

        /// @brief 生成直後に一度だけ呼ばれる関数。同一ティック内で実行される。
        virtual void PostInitProperties();

        /// @brief スポーン後に一度だけ呼ばれる関数。スポーン処理の次のティック前に実行される。
        virtual void BeginPlay();

        /// @brief 毎フレーム呼ばれる関数
        /// @param deltatime
        virtual void Tick(float deltatime);

        /// @brief このオブジェクトの破棄が決定した際に呼ばれる関数
        virtual void OnDestroyed();

        /// @brief このオブジェクトの破棄処理が進行中か取得
        bool HasDestroyMarker() const;

        /// @brief オブジェクトを廃棄待ちにする
        void MarkDestroy();

        /// @brief オブジェクトが指定型かチェック
        /// @tparam T
        /// @return
        template <typename T>
        bool IsA() const
        {
            return typeid(T) == typeid(*this);
        }

        /// @brief オブジェクトの名前を取得
        /// @return
        string GetObjectName();

        /// @brief 所属するSceneを設定
        /// @param _scene
        void SetSpawnedScene(Scene *_scene);

        /// @brief オブジェクト情報を出力
        /// @param os
        virtual void toString(ostream &os) const;

        ostream &logStart(ostream &in) const;

        friend ostream &operator<<(ostream &os, const GameObject &go)
        {
            go.toString(os);
            return os;
        }

        template <typename T>
        static ostream &VectorPrintor(ostream &os, const T &value)
        {
            for (int i = 0; i < value.size(); i++)
            {
                os << C_BLUE << value[i] << C_RESET;
                if (i != value.size())
                    os << ",";
            }
            return os;
        }
    };
    // 文字列生成などの初期化のために使用
    GCLASS_NO_ARG(GameObject)
}