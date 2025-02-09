#pragma once
#include "../../header/EigenHeader.hpp"

class DirectionalLight
{
public:
    DirectionalLight(Vector3f direction, Vector3f color);
    Vector3f direction = Vector3f(0, 0, 1);
    Vector3f color = Vector3f(.8, .8, .8);
};