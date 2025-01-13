#include "GBuffers.hpp"

GBuffers::GBuffers(const int &width, const int &height)
{
    float floatMax = numeric_limits<float>::max();

    this->screenSize = Vector2i(width, height);
    this->normalWS = this->normalVS = this->positionWS = this->positionVS = this->backNormalVS = this->backPositionVS = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->beauty = this->diffuse = this->forward = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->diffuse = this->specular = this->emission = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->depth = this->backDepth = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(floatMax, floatMax, floatMax));
    this->AO = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(1, 1, 1));
    this->reflection = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->SSShadow = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(1, 1, 1));

    this->uv = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));
    this->temp = RenderTarget(
        this->screenSize.x(), this->screenSize.y(), Vector3f(0, 0, 0));

    this->str2rt = {
        {"forward", &forward},
        {"beauty", &beauty},
        {"diffuse", &diffuse},
        {"specular", &specular},
        {"emission", &emission},
        {"depth", &depth},
        {"AO", &AO},
        {"reflection", &reflection},
        {"SSShadow", &SSShadow},
        {"positionWS", &positionWS},
        {"positionVS", &positionVS},
        {"normalWS", &normalWS},
        {"normalVS", &normalVS},
        {"backPositionVS", &backPositionVS},
        {"backNormalVS", &backNormalVS},
        {"backDepth", &backDepth},
        {"uv", &uv},
        {"temp", &temp},
    };
}

GBuffers::~GBuffers()
{
}

void GBuffers::writeAsPNG(const string &filepath,
                          const float positionModValue,
                          const float normalMulValue)
{
    std::filesystem::create_directories(filepath);
    auto appendToFilepath = [&filepath](const string &suffix) -> string
    {
        return filepath + suffix;
    };
    this->beauty.writeAsPNG(appendToFilepath("/out_beauty.png"));
    this->beauty.writeAsPNG(appendToFilepath("/out_beauty.png"));
    this->forward.writeAsPNG(appendToFilepath("/out_forward.png"));
    this->diffuse.writeAsPNG(appendToFilepath("/out_diffuse.png"));
    this->specular.writeAsPNG(appendToFilepath("/out_specular.png"));
    this->emission.writeAsPNG(appendToFilepath("/out_emission.png"));

    (this->depth.Abs() * (1.0 / this->depth.GetMax().x())).writeAsPNG(appendToFilepath("/out_depth.png"));
    (this->AO).writeAsPNG(appendToFilepath("/out_AO.png"));
    (this->reflection).writeAsPNG(appendToFilepath("/out_reflection.png"));
    (this->SSShadow).writeAsPNG(appendToFilepath("/out_SSShadow.png"));

    (this->positionWS.Abs() % positionModValue).writeAsPNG(appendToFilepath("/out_positionWS.png"));
    (this->positionVS.Abs() % positionModValue).writeAsPNG(appendToFilepath("/out_positionVS.png"));
    (this->normalWS * normalMulValue).writeAsPNG(appendToFilepath("/out_normalWS.png"));
    (this->normalVS * normalMulValue).writeAsPNG(appendToFilepath("/out_normalVS.png"));

    (this->backNormalVS * normalMulValue).writeAsPNG(appendToFilepath("/out_backNormalVS.png"));
    (this->backPositionVS.Abs() % positionModValue).writeAsPNG(appendToFilepath("/out_backPositionVS.png"));
    (this->backDepth.Abs() * (1.0 / this->backDepth.GetMax().x())).writeAsPNG(appendToFilepath("/out_backDepth.png"));

    (this->uv).writeAsPNG(appendToFilepath("/out_uv.png"));
    (this->temp).writeAsPNG(appendToFilepath("/out_temp.png"));
}

RenderTarget &GBuffers::getRTFromString(string rtname)
{
    return *(this->str2rt[rtname]);
}