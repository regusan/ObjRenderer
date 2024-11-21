#pragma once
#include <vector>
#include <array>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include "header/EigenHeader.hpp"
using namespace std;

inline const Matrix4f MakeRotMatX(float degree)
{
    float th = degree / 180.0 * M_PI;
    Matrix4f retval = Matrix4f();
    retval << 1, 0, 0, 0,
        0, cos(th), -sin(th), 0,
        0, sin(th), cos(th), 0,
        0, 0, 0, 1;
    return retval;
}
inline const Matrix4f MakeRotMatY(float degree)
{
    float th = degree / 180.0 * M_PI;
    Matrix4f retval = Matrix4f();
    retval << cos(th), 0, sin(th), 0,
        0, 1, 0, 0,
        -sin(th), 0, cos(th), 0,
        0, 0, 0, 1;
    return retval;
}
inline const Matrix4f MakeRotMatZ(float degree)
{
    float th = degree / 180.0 * M_PI;
    Matrix4f retval = Matrix4f();
    retval << cos(th), -sin(th), 0, 0,
        sin(th), cos(th), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;
    return retval;
}
inline const Matrix4f MakeRotMat(Vector3f rotaiton)
{
    return MakeRotMatZ(rotaiton.z()) * MakeRotMatY(rotaiton.y()) * MakeRotMatX(rotaiton.x());
}
inline const Matrix4f MakeMatOffset(Vector3f offset)
{
    Matrix4f retval = Matrix4f();
    retval << 1, 0, 0, offset.x(),
        0, 1, 0, offset.y(),
        0, 0, 1, offset.z(),
        0, 0, 0, 1;
    return retval;
}