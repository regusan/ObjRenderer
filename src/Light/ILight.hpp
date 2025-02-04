#pragma once
#include "../header/EigenHeader.hpp"
#include "../Math/Math3D.hpp"
#include "../header/MathHeader.hpp"
class PixcelInputStandard;
using namespace std;

class ILight
{
private:
    /* data */
public:
    virtual ~ILight() = default;
    virtual Vector3f getIlluminationAtPoint(const PixcelInputStandard &pixcelIn) const = 0;
};
