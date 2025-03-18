#pragma once
#include <sstream>
#include "GameObject.hpp"
#include "Scene.hpp"
#include "../header/EigenHeader.hpp"
#include "../header/MathHeader.hpp"
namespace REngine
{
    using namespace REngine::Input;
    /// @brief シーン上に配置されるオブジェクト
    class Actor : public GameObject
    {
    protected:
        Matrix4f worldMatrix = Matrix4f::Identity();
        Matrix4f localMatrix = Matrix4f::Identity();
        virtual void matUpdate();

    public:
        Vector3f location = Vector3f(0, 0, 0);
        Vector3f rotation = Vector3f(0, 0, 0);
        Vector3f scale = Vector3f(1, 1, 1);

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

        virtual void SetParent(weak_ptr<Actor> parent);
        virtual void DettachParent();

        static json JsonArgParse(json args);
        Matrix4f getWorldMat() const;
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