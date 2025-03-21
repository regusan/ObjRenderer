#pragma once

#include "../../header/EigenHeader.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include "../Actor.hpp"
using namespace Transform;

/// @brief カメラのための仮想クラス
class Camera : public Actor
{
protected:
    Camera(json args) : Actor(args) {}
    Camera() : Actor() {}

private:
public:
    float speed = 30.0f;
    virtual ~Camera() = default; // 仮想デストラクタを定義
};
