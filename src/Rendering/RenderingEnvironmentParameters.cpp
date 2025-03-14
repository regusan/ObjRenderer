#include "RenderingEnvironmentParameters.hpp"

void RenderingEnvironmentParameters::loadFromJson(nlohmann::json jsonObj)
{
    // RenderingSettings
    auto renderingSettings = jsonObj["RenderingSettings"];
    auto lightingSettings = jsonObj["LightingSettings"];
    this->nearClip = renderingSettings["NearClip"];
    this->farClip = renderingSettings["FarClip"];
    this->backfaceCulling = renderingSettings["BackfaceCulling"];
    this->backFaceCullingDirection = renderingSettings["BackfaceCullingDirection"];
    this->screenSize = Vector2i(renderingSettings["Resolution"][0], renderingSettings["Resolution"][1]);
    this->upscaleRate = renderingSettings["UpScaleRate"];
    this->ambientLight = Vector3f(lightingSettings["AmbientLight"][0],
                                  lightingSettings["AmbientLight"][1],
                                  lightingSettings["AmbientLight"][2]);
    this->buffer2Display = renderingSettings["Buffer2Display"];
    // Directional Light
    this->directionalLights.clear();
    auto light = lightingSettings["Lights"]["Light0"];
    this->directionalLights.push_back(DirectionalLight(
        Vector3f(light["Direction"][0], light["Direction"][1], light["Direction"][2]),
        Vector3f(light["Color"][0], light["Color"][1], light["Color"][2])));

    // Fog Settings
    auto fog = lightingSettings["Fog"];
    this->fogColor = Vector3f(
        fog["FogColor"][0],
        fog["FogColor"][1],
        fog["FogColor"][2]);
    this->fogNearFar = Vector2f(
        fog["FogNear"],
        fog["FogFar"]);

    // Quality and CameraMoveMode
    std::string qualityStr = renderingSettings["Quality"];
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
        std::cerr << "Error in config value: \"" << "Quality" << "\": \"" << qualityStr << "\" is not a valid value. Please choose from Wire, Low, Mid, or Cinema." << '\n';
        exit(1);
    }

    this->cameraSpeed = renderingSettings["CameraSpeed"];

    this->timeScale = renderingSettings["timeScale"];

    // HDRI Setup
    static std::string pastHDRIPath = "";
    std::string hdripath = jsonObj["HDRISettings"]["HDRI"];
    if (std::filesystem::exists(hdripath) && pastHDRIPath != hdripath)
    {
        pastHDRIPath = hdripath;
        this->skySphere = RenderTarget(hdripath);
        std::cout << "Generating sky sphere mipmap..." << hdripath << std::endl;
        this->skyMipmap = this->skySphere.value().MakeMipMapBluered(16, 32);
        std::cout << "Sky sphere mipmap generation completed." << std::endl;
        this->skySphereOffset.x() = jsonObj["HDRISettings"]["SkyUVOffsetX"];
    }
}
void RenderingEnvironmentParameters::setCurrentTIme()
{
    this->time = static_cast<float>(clock()) / CLOCKS_PER_SEC * 0.1f * timeScale;
}