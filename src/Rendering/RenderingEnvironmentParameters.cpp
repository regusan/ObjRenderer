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
    directionalLights.push_back(Vector3f(config.GetAsNumeric("Light0DirectionX"),
                                         config.GetAsNumeric("Light0DirectionY"),
                                         config.GetAsNumeric("Light0DirectionZ")));
}