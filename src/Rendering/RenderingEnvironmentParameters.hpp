#pragma once

#include "../header/EigenHeader.hpp"
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
    Vector2i screenSize = Vector2i(1, 1);
    vector<Vector3f> directionalLights = {Vector3f(1, -1, -1).normalized()};
    Vector3f ambientLight = Vector3f(0.1, 0.1, 0.1);
    RenderingEnvironmentParameters(/* args */) {}
    ~RenderingEnvironmentParameters() {}
};