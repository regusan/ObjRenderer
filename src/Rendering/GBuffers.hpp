#pragma once

#include "../header/EigenHeader.hpp"
#include "VertOutputStandard.hpp"
#include "RenderTarget.hpp"
#include <limits>
using namespace std;

class GBuffers
{
private:
    /* data */
public:
    Vector2i screenSize;
    RenderTarget beauty;
    RenderTarget diffuse;

    RenderTarget depth;

    RenderTarget positionWS;

    RenderTarget normalWS;
    RenderTarget normalCS;

    GBuffers(const int &width, const int &height);
    ~GBuffers();
};
GBuffers::GBuffers(const int &width, const int &height)
{
    float floatMax = numeric_limits<float>::max();

    this->screenSize = Vector2i(width, height);
    this->normalWS = this->normalCS = this->positionWS = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->beauty = this->diffuse = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->depth = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(floatMax, floatMax, floatMax));
}

GBuffers::~GBuffers()
{
}