#include "RandomWalkPointLightActor.hpp"

RandomWalkPointLightActor::RandomWalkPointLightActor(json args) : PointLightActor(args)
{
}
RandomWalkPointLightActor::~RandomWalkPointLightActor() {}

void RandomWalkPointLightActor::Tick(float deltatime)
{
    estTime += deltatime;
    float t = estTime;
    Vector3f offset = Vector3f(sin(t * freq.x()), 0, cos(t * freq.z())) * radius * 1;
    this->SetPosition(this->origin + offset);
}

void RandomWalkPointLightActor::BeginPlay()
{
    this->origin = this->location;
}