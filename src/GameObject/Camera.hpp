#pragma once

#include "../header/EigenHeader.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../TransformMat.hpp"
#include <iostream>
#include "Actor.hpp"
using namespace Transform;

/// @brief カメラのための仮想クラス
class Camera : public Actor
{
private:
public:
    virtual ~Camera() = default; // 仮想デストラクタを定義
    virtual void OnUpdateInput(const XEvent &event) = 0;
};
