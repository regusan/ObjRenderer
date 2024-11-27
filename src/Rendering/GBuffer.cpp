#include "GBuffers.hpp"

GBuffers::GBuffers(const int &width, const int &height)
{
    float floatMax = numeric_limits<float>::max();

    this->screenSize = Vector2i(width, height);
    this->normalWS = this->normalVS = this->positionWS = this->positionVS = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->beauty = this->diffuse = this->forward = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->depth = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(floatMax, floatMax, floatMax));
    this->uv = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
}

GBuffers::~GBuffers()
{
}

void GBuffers::writeAsPPM(const string &filepath,
                          const float positionModValue,
                          const float normalMulValue)
{
    this->beauty.writeAsPPM(filepath + string("_beauty.ppm"));
    this->forward.writeAsPPM(filepath + string("_forward.ppm"));
    (this->depth.rtAbs() % positionModValue * normalMulValue).writeAsPPM(filepath + string("_depth.ppm"));
    this->diffuse.writeAsPPM(filepath + string("_diffuse.ppm"));

    (this->positionWS.rtAbs() % positionModValue).writeAsPPM(filepath + string("_positionWS.ppm"));
    (this->positionVS.rtAbs() % positionModValue).writeAsPPM(filepath + string("_positionVS.ppm"));
    (this->normalWS * normalMulValue).writeAsPPM(filepath + string("_normalWS.ppm"));
    (this->normalVS * normalMulValue).writeAsPPM(filepath + string("_normalVS.ppm"));

    (this->uv).writeAsPPM(filepath + string("_uv.ppm"));
}