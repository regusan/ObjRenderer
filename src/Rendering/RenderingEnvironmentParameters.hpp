#pragma once

#include "../header/EigenHeader.hpp"
// #include "../Light/ILight.hpp"
// #include "../Light/RadialPointLight.hpp"
#include "../STL/ConfigParser.hpp"
#include "../Light/DirectionalLight.hpp"

// class ILight;
// class RadialPointLight;
#include <vector>
#include <chrono>
#include <ctime>

enum class RenderingQuality
{
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
    float farClip = 100;
    bool backfaceCulling = false;
    int backFaceCullingDirection = 1;
    Vector2i screenSize = Vector2i(1, 1);
    // vector<RadialPointLight> lights;
    vector<DirectionalLight> directionalLights = {DirectionalLight(Vector3f(1, 1, 1), Vector3f(.8, .8, .8))};

    Vector3f fogColor = Vector3f(1, 1, 1);
    Vector2f fogNearFar = Vector2f(0, 100);

    Matrix4f viewMat = Matrix4f::Identity();
    Vector3f ambientLight = Vector3f(0.1, 0.1, 0.1);
    float time = 0;
    RenderingQuality quality = RenderingQuality::Mid;
    CameraMoveMode cameraMoveMode = CameraMoveMode::FPS;

    /// @brief コンフィグから読み取り
    /// @param config
    void loadFromConfig(ConfigParser config);
    void setCurrentTIme();
    RenderingEnvironmentParameters(/* args */) {}
    ~RenderingEnvironmentParameters() {}
};