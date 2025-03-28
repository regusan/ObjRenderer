#pragma once
#include <sstream>
#include "GameObject.hpp"
#include "Scene.hpp"
#include "../header/EigenHeader.hpp"
#include "../header/MathHeader.hpp"
namespace REngine
{
    using namespace REngine::Input;
    using REngine::Component::ActorComponent;
    /// @brief シーン上に配置されるオブジェクト
    class Actor : public GameObject
    {
    protected:
        Matrix4f worldMatrix = Matrix4f::Identity();
        Matrix4f localMatrix = Matrix4f::Identity();
        virtual void matUpdate();

    public:
        weak_ptr<Actor> parent;
        vector<weak_ptr<Actor>> children;
        vector<weak_ptr<ActorComponent>> components;

        Vector3f location = Vector3f(0, 0, 0);
        Vector3f rotation = Vector3f(0, 0, 0);
        Vector3f scale = Vector3f(1, 1, 1);

        void OnDestroyed();

        virtual void SetLocalRotation(Vector3f rotate);
        virtual void SetLocalPosition(Vector3f position);
        virtual void SetLocalScale(Vector3f scale);

        /*
        virtual void SetWorldRotation(Vector3f rotate);
        virtual void SetWorldPosition(Vector3f position);
        virtual void SetWorldScale(Vector3f scale);
        */

        virtual Vector3f GetWorldRotation() const;
        virtual Vector3f GetWorldPosition() const;
        virtual Vector3f GetWorldScale() const;
        virtual Vector3f GetLocalRotation() const;
        virtual Vector3f GetLocalPosition() const;
        virtual Vector3f GetLocalScale() const;

        // ==========親子系===================-
        /// @brief 自身の親を設定
        /// @param parent
        virtual void SetParent(weak_ptr<Actor> parent);

        /// @brief 自身の親から切り離す
        virtual void DettachParent();

        /// @brief オブジェクトを子供にする
        /// @param child
        virtual void AddChild(weak_ptr<Actor> child);

        /// @brief 子供オブジェクトの親子関係を解除する
        /// @param child
        virtual void DettachChild(weak_ptr<Actor> child);

        //============コンポーネント系===============
        /// @brief コンポーネントを追加する
        /// @param component
        virtual void AddComponent(weak_ptr<ActorComponent> component);
        /// @brief コンポーネントを削除する
        /// @param component
        virtual void RemoveComponent(weak_ptr<ActorComponent> component);

        /// @brief 所有しているコンポーネントを指定クラスで全て取得
        /// @tparam T
        /// @return
        template <typename T>
        inline vector<weak_ptr<T>> GetComponentsOfClass()
        {
            vector<weak_ptr<T>> result;
            for (auto &component : components)
            {
                if (auto sharedComponent = component.lock())
                {
                    if (auto castedComponent = dynamic_pointer_cast<T>(sharedComponent))
                    {
                        result.push_back(castedComponent);
                    }
                }
            }
            return result;
        }
        /// @brief 所有しているコンポーネントを指定クラスで取得
        /// @tparam T
        /// @return
        template <typename T>
        inline weak_ptr<T> GetComponentOfClass()
        {
            for (auto &component : components)
            {
                if (auto sharedComponent = component.lock())
                {
                    if (auto castedComponent = dynamic_pointer_cast<T>(sharedComponent))
                    {
                        return castedComponent;
                    }
                }
            }
            return weak_ptr<T>();
        }

        static json JsonArgParse(json args);
        Matrix4f getWorldMat() const;
        virtual void toString(ostream &os) const override;
        Actor(/* args */);
        Actor(json args);
        ~Actor();
    };
    GCLASS(Actor)
}

/// @brief エラー処理を定義したJson
class SafeJson : public json
{
public:
    using json::json; // 基底クラスのコンストラクタを継承
    SafeJson(const json &other) : json(other) {}
    // オーバーロードされた [] 演算子
    const SafeJson operator[](const string &key) const
    {
        if (!this->contains(key))
        {
            stringstream s;
            s << "キー '" << key << "' が存在しません" << endl
              << this->dump() << endl;
            throw runtime_error(s.str());
        }
        return SafeJson(json::operator[](key));
    }
    const SafeJson operator[](const char *key) const
    {
        return SafeJson::operator[](string(key));
    }
    const SafeJson operator[](const int index) const
    {
        return SafeJson(json::operator[](index));
    }
};