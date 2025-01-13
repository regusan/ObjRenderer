#pragma once

#include "../header/EigenHeader.hpp"
#include "../Models/Material.hpp"
#include "../Rendering/RenderingEnvironmentParameters.hpp"

/// @brief 頂点シェーダーの入力データクラス
class VertInputStandard
{
private:
    /* data */
public:
    VertInputStandard(RenderingEnvironmentParameters &environment) : environment(environment) {}
    ~VertInputStandard() {}
    float nearClip = 1;
    float farClip = 100;
    bool backfaceCulling = false;
    Vector2i screenSize = Vector2i(1, 0);

    RenderingEnvironmentParameters &environment;

    Vector4f position = Vector4f(0, 0, 0, 1);
    Vector4f normal = Vector4f(1, 0, 0, 1);
    Vector2f uv = Vector2f(0, 0);
    Matrix4f modelMat = Matrix4f::Identity();
    Matrix4f viewMat = Matrix4f::Identity();
    Vector4f vertColor = Vector4f(1, 0, 1, 1);

    Material *material = nullptr;
};