#pragma once
#include "ILight.hpp"

class RadialPointLight : public ILight
{
private:
    /* data */
public:
    float radius;
    Matrix4f modelMat = Matrix4f::Identity();
    Vector3f color = Vector3f(1, 0, 1);
    RadialPointLight() {};
    ~RadialPointLight() {};
    Vector3f getIlluminationAtPoint(const PixcelInputStandard &pixcelIn) const override;
};