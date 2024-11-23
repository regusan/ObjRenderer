#pragma once
#include "header/ShaderHeader.hpp"
#include "header/EigenHeader.hpp"
using namespace std;

class TransformMat
{
private:
    /* data */
public:
    TransformMat(/* args */);
    ~TransformMat();
    inline static const TransformMat MakeRotMatX(float degree)
    {
        float th = degree / 180.0 * M_PI;
        TransformMat retval = TransformMat();
        /*
        retval << 1, 0, 0, 0,
            0, cos(th), -sin(th), 0,
            0, sin(th), cos(th), 0,
            0, 0, 0, 1;
            */
        return retval;
    }
};

TransformMat::TransformMat(/* args */)
{
}

TransformMat::~TransformMat()
{
}
