
#include "LightSpawnerActor.hpp"

LightSpawnActor::LightSpawnActor(const json &args) : Actor(args)
{
}

void LightSpawnActor::BeginPlay()
{
    if (this->scene)
    {
        for (size_t i = 0; i < 1000; i++)
        {
            Vector3f spawnPos = Vector3f::Random() * 8;
            spawnPos.y() = 0.2;
            this->scene->SpawnActorOfClass<RandomWalkPointLightActor>(spawnPos, 0.3f, Vector3f::Random().normalized() * 5);
        }
    }
}
