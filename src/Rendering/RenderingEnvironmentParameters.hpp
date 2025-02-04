#pragma once

#include "../header/EigenHeader.hpp"
// #include "../Light/ILight.hpp"
// #include "../Light/RadialPointLight.hpp"
#include "../Light/DirectionalLight.hpp"
#include "../Rendering/RenderTarget.hpp"

#include "../Math/Math3D.hpp"
// class ILight;
// class RadialPointLight;
#include <vector>
#include <chrono>
#include <ctime>
#include <optional>
#include <nlohmann/json.hpp>

enum class RenderingQuality
{
    Wire,
    Low,
    Mid,
    Cinema,
};
enum class CameraMoveMode
{
    FPS,
    TurnTable,
};
using namespace std;
class RenderingEnvironmentParameters
{
private:
    /* data */
public:
    float nearClip = 1;
    float farClip = 1000;
    bool backfaceCulling = false;
    int backFaceCullingDirection = -1;
    Vector2i screenSize = Vector2i(512, 512);
    // vector<RadialPointLight> lights;
    vector<DirectionalLight> directionalLights = {DirectionalLight(Vector3f(1, 1, 1), Vector3f(.8, .8, .8))};

    Vector3f fogColor = Vector3f(0, 0, 0);
    Vector2f fogNearFar = Vector2f(0, 100);

    Matrix4f viewMat = Matrix4f::Identity();
    Vector3f ambientLight = Vector3f(0.1, 0.1, 0.1);
    float time = 0;
    RenderingQuality quality = RenderingQuality::Mid;
    string buffer2Display = "beauty";
    CameraMoveMode cameraMoveMode = CameraMoveMode::FPS;

    optional<RenderTarget> skySphere;
    vector<RenderTarget> skyMipmap;
    Vector2f skySphereOffset = Vector2f(0, 0);

    int MaxTesselleateCount = 5;

    void loadFromJson(nlohmann::json jsonObj);
    void setCurrentTIme();
    RenderingEnvironmentParameters(/* args */) {}
    ~RenderingEnvironmentParameters() {}

    friend ostream &operator<<(ostream &os, const RenderingEnvironmentParameters &env)
    {
        os << "Rendering Environment Parameters:" << endl;
        os << "  NearClip: " << env.nearClip << endl;
        os << "  FarClip: " << env.farClip << endl;
        os << "  BackfaceCulling: " << (env.backfaceCulling ? "true" : "false") << endl;
        os << "  BackfaceCullingDirection: " << env.backFaceCullingDirection << endl;
        os << "  ScreenSize: (" << env.screenSize.x() << ", " << env.screenSize.y() << ")" << endl;

        os << "  DirectionalLights: " << endl;
        for (const auto &light : env.directionalLights)
        {
            os << "    Direction: ("
               << light.direction.x() << ", "
               << light.direction.y() << ", "
               << light.direction.z() << ") "
               << "Color: ("
               << light.color.x() << ", "
               << light.color.y() << ", "
               << light.color.z() << ")" << endl;
        }

        os << "  FogColor: (" << env.fogColor.x() << ", " << env.fogColor.y() << ", " << env.fogColor.z() << ")" << endl;
        os << "  FogNearFar: (" << env.fogNearFar.x() << ", " << env.fogNearFar.y() << ")" << endl;
        os << "  AmbientLight: (" << env.ambientLight.x() << ", " << env.ambientLight.y() << ", " << env.ambientLight.z() << ")" << endl;
        os << "  Time: " << env.time << endl;
        os << "  Quality: " << (env.quality == RenderingQuality::Wire ? "Wire" : (env.quality == RenderingQuality::Low ? "Low" : (env.quality == RenderingQuality::Mid ? "Mid" : "Cinema"))) << endl;
        os << "  Buffer2Display: " << env.buffer2Display << endl;
        os << "  CameraMoveMode: " << (env.cameraMoveMode == CameraMoveMode::FPS ? "FPS" : "TurnTable") << endl;

        if (env.skySphere)
        {
            os << "  SkySphere: Exists" << endl;
        }
        else
        {
            os << "  SkySphere: Not set" << endl;
        }

        os << "  MaxTessellationCount: " << env.MaxTesselleateCount << endl;

        return os;
    }
};