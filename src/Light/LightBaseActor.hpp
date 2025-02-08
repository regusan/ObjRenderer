#pragma once

#include "../GameObject/Actor.hpp"
using namespace Transform;

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

    LightBaseActor(json args) : Actor(args)
    {
        this->color.x() = args["color"][0];
        this->color.y() = args["color"][1];
        this->color.z() = args["color"][2];
    };
    ~LightBaseActor() {};
};
// GCLASS(LightBaseActor)
