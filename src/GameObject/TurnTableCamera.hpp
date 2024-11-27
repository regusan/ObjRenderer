#pragma once

#include "../header/EigenHeader.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../TransformMat.hpp"
#include <iostream>
#include "Actor.hpp"
using namespace Transform;
class TurnTableCamera : Actor
{
private:
    Matrix4f mat;
    float radius = 10;
    void matUpdate();

public:
    TurnTableCamera(/* args */);
    ~TurnTableCamera();
    void SetRadius(float radius);
    void SetRotation(Vector3f rotate);
    void SetPosition(Vector3f position);
    void OnUpdateInput(const XEvent &event);

    Matrix4f getMat();
};
