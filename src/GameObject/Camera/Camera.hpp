#pragma once

#include "../../header/EigenHeader.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include "../Actor.hpp"
using namespace Transform;

namespace REngine
{
    /// @brief カメラのための仮想クラス
    class Camera : public Actor
    {
    protected:
    private:
    public:
        float speed = 30.0f;
        Camera(json args) : Actor(args) {}
        Camera() : Actor() {}
        ~Camera() {}
    };
    GCLASS(Camera);
}