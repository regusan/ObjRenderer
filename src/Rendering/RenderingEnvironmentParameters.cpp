#include "RenderingEnvironmentParameters.hpp"

void RenderingEnvironmentParameters::loadFromConfig(ConfigParser config)
{
    this->nearClip = config.GetAsNumeric("NearClip");
    this->farClip = config.GetAsNumeric("FarClip");
    this->backfaceCulling = config.GetAsBool("BackfaceCulling");
    this->backFaceCullingDirection = config.GetAsNumeric("BackfaceCullingDirection");
    this->screenSize.x() = config.GetAsNumeric("ResolutionX");
    this->screenSize.y() = config.GetAsNumeric("ResolutionY");
    this->ambientLight = Vector3f(config.GetAsNumeric("AmbientLightR"),
                                  config.GetAsNumeric("AmbientLightG"),
                                  config.GetAsNumeric("AmbientLightB"));
    this->directionalLights.clear();
    this->directionalLights.push_back(DirectionalLight(
        Vector3f(config.GetAsNumeric("Light0DirectionX"),
                 config.GetAsNumeric("Light0DirectionY"),
                 config.GetAsNumeric("Light0DirectionZ")),
        Vector3f(config.GetAsNumeric("Light0ColorR"),
                 config.GetAsNumeric("Light0ColorG"),
                 config.GetAsNumeric("Light0ColorB"))));
    this->fogColor = Vector3f(config.GetAsNumeric("FogColorR"),
                              config.GetAsNumeric("FogColorG"),
                              config.GetAsNumeric("FogColorB"));
    this->fogNearFar = Vector2f(config.GetAsNumeric("FogNear"), config.GetAsNumeric("FogFar"));

    string qualityStr = config.GetAsString("Quality");
    if (qualityStr == "Low")
        this->quality = RenderingQuality::Low;
    else if (qualityStr == "Mid")
        this->quality = RenderingQuality::Mid;
    else if (qualityStr == "Cinema")
        this->quality = RenderingQuality::Cinema;
    else
    {
        std::cerr << "コンフィグ値エラー:\"" << "Quality" << "\":\"" << qualityStr << "\"はEnum::Qualityでない値。Low,Mid,Cinemaの中から選んでください。" << '\n';
        exit(1);
    }

    string cameraMoveModeStr = config.GetAsString("CameraMoveMode");
    if (cameraMoveModeStr == "FPS")
        this->cameraMoveMode = CameraMoveMode::FPS;
    if (cameraMoveModeStr == "TurnTable")
        this->cameraMoveMode = CameraMoveMode::TurnTable;

    this->skySphereSpecular = RenderTarget(config.GetAsString("HDRI"));
    vector<RenderTarget::DownSampleData> dsd = {
        RenderTarget::DownSampleData(this->skySphereSpecular->getScreenSize() / 4, 17),
        RenderTarget::DownSampleData(this->skySphereSpecular->getScreenSize() / 8, 33),
        RenderTarget::DownSampleData(this->skySphereSpecular->getScreenSize() / 16, 65),
    };
    vector<RenderTarget> downSampledBuffers = this->skySphereSpecular->GausiannBlurWithDownSample(dsd);
    RenderTarget avg = RenderTarget(this->skySphereSpecular->getScreenSize().x(), this->skySphereSpecular->getScreenSize().y());
    for (int y = 0; y < avg.getScreenSize().y(); y++)
        for (int x = 0; x < avg.getScreenSize().x(); x++)
        {
            // ダウンサンプリングしたバッファを合成
            Vector3f sum = Vector3f(0, 0, 0);
            for (size_t i = 0; i < downSampledBuffers.size(); i++)
                sum += downSampledBuffers[i].SampleColor(x, y);
            avg.PaintPixel(x, y, sum / downSampledBuffers.size());
        }
    this->skySphereDiffuse = avg;
}

void RenderingEnvironmentParameters::setCurrentTIme()
{
    this->time = static_cast<float>(clock()) / CLOCKS_PER_SEC * 1000;
}