#pragma once

#include "../Actor.hpp"
using namespace Transform;
namespace REngine::Light
{
    /// @brief ライトにまつわる仮想クラス
    class LightBaseActor : public Actor
    {
    protected:
        Vector3f color = Vector3f(1, 0, 0);

    public:
        /// @brief ある地点でのライトの照度を求める関数
        /// @param positionWS その地点の位置
        /// @param normalWS その地点の法線
        /// @return 照度
        virtual Vector3f lightSDF(const Vector3f &positionWS, const Vector3f &normalWS) { return Vector3f::Zero(); };

        virtual float GetBoundingSphereRadius() { return 0.0f; }
        LightBaseActor() {}
        /// @brief Sceneファイル読み取り時にJsonとして渡される
        /// @param args
        LightBaseActor(json args) : Actor(args)
        {
            this->color.x() = args["color"][0];
            this->color.y() = args["color"][1];
            this->color.z() = args["color"][2];
        };
        ~LightBaseActor() {};
    };

    // GCLASSマクロで登録
    GCLASS(LightBaseActor)
}