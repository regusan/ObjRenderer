#pragma once

#include "../header/EigenHeader.hpp"
// #include "../Light/ILight.hpp"
// #include "../Light/RadialPointLight.hpp"
#include "../STL/ConfigParser.hpp"

// class ILight;
// class RadialPointLight;
#include <vector>
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
    vector<Vector3f> directionalLights = {Vector3f(1, -1, -1).normalized()};
    Matrix4f viewMat = Matrix4f::Identity();
    Vector3f ambientLight = Vector3f(0.1, 0.1, 0.1);

    /// @brief コンフィグから読み取り
    /// @param config
    void loadFromConfig(ConfigParser config);
    RenderingEnvironmentParameters(/* args */) {}
    ~RenderingEnvironmentParameters() {}
};