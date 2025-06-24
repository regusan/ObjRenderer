#pragma once
namespace REngine::Collision::RaceGame
{
    enum class RaceGameCollisionLayer : char
    {
        NoCollision = 0,
        Default = 1 << 0,
        PlayerCar = 1 << 1,
        AICar = 1 << 2,
        TrackBarrier = 1 << 3,
        TrackSurface = 1 << 4,
        Obstacle = 1 << 5,
        Pickup = 1 << 6,
        ALL = 0b1111111,
    };
}