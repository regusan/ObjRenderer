#include "GBuffers.hpp"

GBuffers::GBuffers(const int &width, const int &height)
{
    float floatMax = numeric_limits<float>::max();
    str2rt = {
        {"forward", &forward},
        {"beauty", &beauty},

        {"diffuse", &diffuse},
        {"specular", &specular},
        {"ORM", &ORM},

        {"depth", &depth},

        {"AO", &AO},
        {"reflection", &reflection},
        {"SSShadow", &SSShadow},
        {"irradiance", &irradiance},

        {"lightDomain", &lightDomain},
        {"lightDepth", &lightDepth},
        {"lightBackDepth", &lightBackDepth},
        {"lightPositionVS", &lightPositionVS},

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

    this->screenSize = Vector2i(width, height);
    Vector2i ss = this->screenSize;

    this->normalWS = this->normalVS = this->positionWS = this->positionVS =
        this->backNormalVS = this->backPositionVS = this->lightPositionVS = RenderTarget(
            ss.x(), ss.y(), Vector3f(0, 0, 0));

    this->beauty = this->diffuse = this->forward = RenderTarget(
        ss.x(), ss.y(), Vector3f(0, 0, 0));
    this->diffuse = this->specular = this->irradiance = RenderTarget(
        ss.x(), ss.y(), Vector3f(0, 0, 0));

    this->ORM = RenderTarget(ss.x(), ss.y(), Vector3f(1, 0, 0));

    this->depth = this->backDepth = this->lightDepth = this->lightBackDepth = RenderTarget(
        ss.x(), ss.y(), Vector3f(floatMax, floatMax, floatMax));

    this->AO = RenderTarget(
        ss.x(), ss.y(), Vector3f(1, 1, 1));
    this->reflection = RenderTarget(
        ss.x(), ss.y(), Vector3f(0, 0, 0));
    this->SSShadow = RenderTarget(
        ss.x(), ss.y(), Vector3f(1, 1, 1));

    this->lightDomain = RenderTarget(
        ss.x(), ss.y(), Vector3f(0, 0, 0));

    this->uv = RenderTarget(
        ss.x(), ss.y(), Vector3f(0, 0, 0));
    this->temp = RenderTarget(
        ss.x(), ss.y(), Vector3f(0, 0, 0));
}

GBuffers::~GBuffers()
{
}

void GBuffers::Clear()
{
    for (const auto &[name, target] : this->str2rt)
    {
        target->Fill(target->resetColor);
    }
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
    this->forward.writeAsPNG(appendToFilepath("/forward.png"));
    this->beauty.writeAsPNG(appendToFilepath("/beauty.png"));

    this->diffuse.writeAsPNG(appendToFilepath("/diffuse.png"));
    this->specular.writeAsPNG(appendToFilepath("/specular.png"));
    this->ORM.writeAsPNG(appendToFilepath("/ORM.png"));

    (this->AO).writeAsPNG(appendToFilepath("/AO.png"));
    (this->reflection).writeAsPNG(appendToFilepath("/reflection.png"));
    (this->SSShadow).writeAsPNG(appendToFilepath("/SSShadow.png"));
    this->irradiance.writeAsPNG(appendToFilepath("/irradiance.png"));

    this->lightDomain.writeAsPNG(appendToFilepath("/lightDomain.png"));
    (this->lightDepth.Abs() * (1.0 / this->lightDepth.GetMax().x())).writeAsPNG(appendToFilepath("/lightDepth.png"));
    (this->lightBackDepth.Abs() * (1.0 / this->lightBackDepth.GetMax().x())).writeAsPNG(appendToFilepath("/lightBackDepth.png"));
    this->lightPositionVS.writeAsPNG(appendToFilepath("/lightPositionVS.png"));

    (this->depth.Abs() * (1.0 / this->depth.GetMax().x())).writeAsPNG(appendToFilepath("/depth.png"));

    (this->positionWS.Abs() % positionModValue).writeAsPNG(appendToFilepath("/positionWS.png"));
    (this->positionVS.Abs() % positionModValue).writeAsPNG(appendToFilepath("/positionVS.png"));

    (this->normalWS * normalMulValue).writeAsPNG(appendToFilepath("/normalWS.png"));
    (this->normalVS * normalMulValue).writeAsPNG(appendToFilepath("/normalVS.png"));

    (this->backNormalVS * normalMulValue).writeAsPNG(appendToFilepath("/backNormalVS.png"));
    (this->backPositionVS.Abs() % positionModValue).writeAsPNG(appendToFilepath("/backPositionVS.png"));
    (this->backDepth.Abs() * (1.0 / this->backDepth.GetMax().x())).writeAsPNG(appendToFilepath("/backDepth.png"));

    (this->uv).writeAsPNG(appendToFilepath("/uv.png"));
    (this->temp).writeAsPNG(appendToFilepath("/temp.png"));
}

RenderTarget &GBuffers::getRTFromString(string rtname)
{
    auto it = this->str2rt.find(rtname);
    if (it != this->str2rt.end())
    {
        if (this->str2rt[rtname] == NULL)
        {
            cerr << "\"RenderTarget:" << rtname << "\"は存在しないキー。";
            exit(1);
        }
        return *this->str2rt[rtname];
    }
    else
    {
        cerr << "\"RenderTarget:" << rtname << "\"は存在しないキー。";
        exit(1);
    }
}