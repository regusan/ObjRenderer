#include "DirectionalLight.hpp"
namespace REngine::Light
{
    DirectionalLight::DirectionalLight(Vector3f direction, Vector3f color)
    {
        this->direction = direction.normalized();
        this->color = color;
    }
}