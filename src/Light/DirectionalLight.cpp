#include "DirectionalLight.hpp"

DirectionalLight::DirectionalLight(Vector3f direction, Vector3f color)
{
    this->direction = direction.normalized();
    this->color = color;
}