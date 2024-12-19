#include "RadialPointLight.hpp"
#include "../shader/PixcelInputStandard.hpp"

Vector3f RadialPointLight::getIlluminationAtPoint(const PixcelInputStandard &pixcelIn) const
{
    float distance = pixcelIn.positionWS.norm();
    float illuminance = clamp<float>((this->radius - distance) / this->radius, 0, 1);
    return this->color * illuminance;
}