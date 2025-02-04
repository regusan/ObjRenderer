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
    if (qualityStr == "Wire")
        this->quality = RenderingQuality::Wire;
    else if (qualityStr == "Low")
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
    static string pastHDRIPath = "";
    // HDRIが存在したら
    if (std::filesystem::exists(config.GetAsString("HDRI")) && pastHDRIPath != config.GetAsString("HDRI"))
    {
        pastHDRIPath = config.GetAsString("HDRI");
        this->skySphere = RenderTarget(config.GetAsString("HDRI"));
        cout << "天球ミップマップの生成中..." << endl;
        this->skyMipmap = this->skySphere.value().MakeMipMapBluered(16, 32);
        cout << "天球ミップマップの生成完了" << endl;
        this->skySphereOffset.x() = config.GetAsNumeric("SkyUVOffsetX");

        /*
        // ライト向きをHDRIと同期する
        // TODO::正しく方向を取得できない！
        RenderTarget &sampler = this->skyMipmap[this->skyMipmap.size() - 2];
        auto compare = [](const Vector3f &a, const Vector3f &b)
        { return a.norm() < b.norm(); };
        auto maxIter = std::max_element(sampler.GetArray().begin(), sampler.GetArray().end(), compare);
        int index = std::distance(sampler.GetArray().begin(), maxIter);

        Vector2f maxUV = Vector2f(
                             (float)(index % sampler.getScreenSize().x()) / sampler.getScreenSize().x(),
                             (float)(index / sampler.getScreenSize().x()) / sampler.getScreenSize().y()) -
                         this->skySphereOffset;

        this->directionalLights[0].direction = TextureMath::PolarUVToRectangular(1, maxUV);
        */
    }
}

void RenderingEnvironmentParameters::setCurrentTIme()
{
    this->time = static_cast<float>(clock()) / CLOCKS_PER_SEC * 1000;
}