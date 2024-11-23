#include "TransformMat.hpp"
inline static const TransformMat TransformMat::MakeRotMatX(float degree)
{
    float th = degree / 180.0 * M_PI;
    TransformMat retval = TransformMat();
    retval << 1, 0, 0, 0,
        0, cos(th), -sin(th), 0,
        0, sin(th), cos(th), 0,
        0, 0, 0, 1;
    return retval;
}