#include "RenderingEnvironmentParameters.hpp"

void RenderingEnvironmentParameters::loadFromConfig(ConfigParser config)
{
    nearClip = config.GetAsNumeric("NearClip");
    farClip = config.GetAsNumeric("FarClip");
    backfaceCulling = config.GetAsBool("BackfaceCulling");
    backFaceCullingDirection = config.GetAsNumeric("BackfaceCullingDirection");
    screenSize.x() = config.GetAsNumeric("ResolutionX");
    screenSize.y() = config.GetAsNumeric("ResolutionY");
    ambientLight = Vector3f(config.GetAsNumeric("AmbientLightR"),
                            config.GetAsNumeric("AmbientLightG"),
                            config.GetAsNumeric("AmbientLightB"));
    directionalLights.clear();
    directionalLights.push_back(DirectionalLight(
        Vector3f(config.GetAsNumeric("Light0DirectionX"),
                 config.GetAsNumeric("Light0DirectionY"),
                 config.GetAsNumeric("Light0DirectionZ")),
        Vector3f(config.GetAsNumeric("Light0ColorR"),
                 config.GetAsNumeric("Light0ColorG"),
                 config.GetAsNumeric("Light0ColorB"))));
}

void RenderingEnvironmentParameters::setCurrentTIme()
{
    time = static_cast<float>(clock()) / CLOCKS_PER_SEC * 1000;
}