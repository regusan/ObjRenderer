#include "RandomWalkPointLightActor.hpp"
namespace REngine::Light
{
    RandomWalkPointLightActor::RandomWalkPointLightActor(json args) : PointLightActor(args)
    {
        this->freq = (Vector3f::Random() + Vector3f::Ones()) * 1;
    }
    RandomWalkPointLightActor::RandomWalkPointLightActor() : PointLightActor()
    {
        this->freq = (Vector3f::Random() + Vector3f::Ones()) * 1;
    }
    RandomWalkPointLightActor::~RandomWalkPointLightActor() {}

    void RandomWalkPointLightActor::Tick(float deltatime)
    {
        estTime += deltatime;
        float t = estTime;
        Vector3f offset = Vector3f(sin(t * freq.x()), 0, cos(t * freq.z())) * radius * 1;
        this->SetLocalPosition(this->origin + offset);
    }

    RandomWalkPointLightActor::RandomWalkPointLightActor(Vector3f pos, float radius, Vector3f color) : RandomWalkPointLightActor()
    {
        this->color = color;
        this->radius = radius;
        this->SetLocalPosition(pos);
        this->origin = pos;
    }
    void RandomWalkPointLightActor::BeginPlay()
    {
        this->origin = this->location;
    }
}