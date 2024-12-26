#include "GBuffers.hpp"

GBuffers::GBuffers(const int &width, const int &height)
{
    float floatMax = numeric_limits<float>::max();

    this->screenSize = Vector2i(width, height);
    this->normalWS = this->normalVS = this->positionWS = this->positionVS = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->beauty = this->diffuse = this->forward = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->diffuse = this->specular = this->emission = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->depth = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(floatMax, floatMax, floatMax));
    this->uv = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));

    this->str2rt = {
        {"forward", &forward},
        {"beauty", &beauty},
        {"diffuse", &diffuse},
        {"specular", &specular},
        {"emission", &emission},
        {"depth", &depth},
        {"positionWS", &positionWS},
        {"positionVS", &positionVS},
        {"normalWS", &normalWS},
        {"normalVS", &normalVS},
        {"uv", &uv},
    };
}

GBuffers::~GBuffers()
{
}

void GBuffers::writeAsPPM(const string &filepath,
                          const float positionModValue,
                          const float normalMulValue)
{
    auto appendToFilepath = [&filepath](const string &suffix) -> string
    {
        return filepath + suffix;
    };
    this->beauty.writeAsPPM(appendToFilepath("_beauty.ppm"));
    this->forward.writeAsPPM(appendToFilepath("_forward.ppm"));
    (this->depth.Abs() * (1.0 / this->depth.GetMax().x())).writeAsPPM(appendToFilepath("_depth.ppm"));
    this->diffuse.writeAsPPM(appendToFilepath("_diffuse.ppm"));
    this->specular.writeAsPPM(appendToFilepath("_specular.ppm"));
    this->emission.writeAsPPM(appendToFilepath("_emission.ppm"));

    (this->positionWS.Abs() % positionModValue).writeAsPPM(appendToFilepath("_positionWS.ppm"));
    (this->positionVS.Abs() % positionModValue).writeAsPPM(appendToFilepath("_positionVS.ppm"));
    (this->normalWS * normalMulValue).writeAsPPM(appendToFilepath("_normalWS.ppm"));
    (this->normalVS * normalMulValue).writeAsPPM(appendToFilepath("_normalVS.ppm"));

    (this->uv).writeAsPPM(appendToFilepath("_uv.ppm"));
}

RenderTarget &GBuffers::getRTFromString(string rtname)
{
    return *(this->str2rt[rtname]);
}