#pragma once

#include <vector>
#include <array>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include "../header/EigenHeader.hpp"
#include "../header/ShaderHeader.hpp"
#include "Material.hpp"
using namespace std;
class Model
{
private:
    void loadObj(const filesystem::path &filepath);

public:
    Model(/* args */);
    ~Model();
    filesystem::path loadedFilepath = "";
    /// @brief 頂点座標
    vector<Vector4f> verts;
    /// @brief 変換後の頂点
    vector<VertOutputStandard> transformed;
    /// @brief 法線
    vector<Vector4f> vertNormals;
    /// @brief 変換後の法線
    vector<VertOutputStandard> t_normals;
    /// @brief UV座標
    vector<Vector2f> uv;
    /// @brief 面を構成する頂点ID
    vector<vector<int>> facesID;
    /// @brief UVを構成するID
    vector<vector<int>> uvID;
    /// @brief 法線を構成するID
    vector<vector<int>> normalID;

    map<string, Material> materials;
    void LoadModelFromFIle(const filesystem::path &filepath);
    const string toString();
    void transformVerts(
        const VertInputStandard &in,
        const VertOutputStandard (*vertFunc)(const VertInputStandard &in))
    {
        VertInputStandard vin = in;
        transformed.clear();
        for (size_t i = 0; i < this->verts.size(); i++)
        {
            vin.position = this->verts[i];
            vin.normal = this->vertNormals[i];
            transformed.push_back(vertFunc(vin));
        }
    }
};
