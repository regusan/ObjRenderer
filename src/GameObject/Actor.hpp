#pragma once
#include <sstream>
#include "GameObject.hpp"
#include "Scene.hpp"
#include "../header/EigenHeader.hpp"
#include "../header/MathHeader.hpp"
namespace REngine
{
    /// @brief シーン上に配置されるオブジェクト
    class Actor : public GameObject
    {
    protected:
        Matrix4f mat = Matrix4f::Identity();
        virtual void matUpdate();

    public:
        weak_ptr<Actor> parent;
        vector<weak_ptr<Actor>> children;

        Vector3f location = Vector3f(0, 0, 0);
        Vector3f rotation = Vector3f(0, 0, 0);
        Vector3f scale = Vector3f(1, 1, 1);
        virtual void SetRotation(Vector3f rotate);
        virtual void SetPosition(Vector3f position);
        virtual void SetScale(Vector3f scale);

        // 親子系
        virtual void SetParent(weak_ptr<Actor> parent);
        virtual void DettachParent();
        virtual void AddChild(weak_ptr<Actor> child);
        virtual void DettachChild(weak_ptr<Actor> child);

        virtual void OnDestroyed() override;

        static json JsonArgParse(json args);
        Matrix4f getMat();
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