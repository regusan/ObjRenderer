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
    this->AO = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(1, 1, 1));
    this->uv = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));

    this->str2rt = {
        {"forward", &forward},
        {"beauty", &beauty},
        {"diffuse", &diffuse},
        {"specular", &specular},
        {"emission", &emission},
        {"depth", &depth},
        {"AO", &AO},
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
    std::filesystem::create_directory(filepath);
    auto appendToFilepath = [&filepath](const string &suffix) -> string
    {
        return filepath + suffix;
    };
    this->beauty.writeAsPPM(appendToFilepath("/out_beauty.ppm"));
    this->forward.writeAsPPM(appendToFilepath("/out_forward.ppm"));
    this->diffuse.writeAsPPM(appendToFilepath("/out_diffuse.ppm"));
    this->specular.writeAsPPM(appendToFilepath("/out_specular.ppm"));
    this->emission.writeAsPPM(appendToFilepath("/out_emission.ppm"));

    (this->depth.Abs() * (1.0 / this->depth.GetMax().x())).writeAsPPM(appendToFilepath("/out_depth.ppm"));
    (this->AO).writeAsPPM(appendToFilepath("/out_AO.ppm"));

    (this->positionWS.Abs() % positionModValue).writeAsPPM(appendToFilepath("/out_positionWS.ppm"));
    (this->positionVS.Abs() % positionModValue).writeAsPPM(appendToFilepath("/out_positionVS.ppm"));
    (this->normalWS * normalMulValue).writeAsPPM(appendToFilepath("/out_normalWS.ppm"));
    (this->normalVS * normalMulValue).writeAsPPM(appendToFilepath("/out_normalVS.ppm"));

    (this->uv).writeAsPPM(appendToFilepath("/out_uv.ppm"));
}

RenderTarget &GBuffers::getRTFromString(string rtname)
{
    return *(this->str2rt[rtname]);
}