#include "GBuffers.hpp"

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

void GBuffers::writeAsPPM(const string &filepath)
{
    this->beauty.writeAsPPM(filepath + string("_beuty.ppm"));
    this->depth.writeAsPPM(filepath + string("_depth.ppm"));
    this->diffuse.writeAsPPM(filepath + string("_diffuse.ppm"));
    (this->normalCS * 255).writeAsPPM(filepath + string("_normalCS.ppm"));
    (this->normalWS * 255).writeAsPPM(filepath + string("_normalWS.ppm"));
    (this->positionWS).writeAsPPM(filepath + string("_positionWS.ppm"));
}