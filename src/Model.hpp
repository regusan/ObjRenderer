#pragma once

#include <vector>
#include <array>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include "header/EigenHeader.hpp"
using namespace std;
class Model
{
private:
public:
    Model(/* args */);
    ~Model();
    string loadedFilepath = string("NULL");
    /// @brief 頂点座標
    vector<Vector4f> verts;
    /// @brief 法線
    vector<Vector3f> vertNormals;
    /// @brief UV座標
    vector<Vector2f> uv;
    /// @brief 面を構成する頂点ID
    vector<vector<int>> facesID;
    /// @brief UVを構成するID
    vector<vector<int>> uvID;
    /// @brief 法線を構成するID
    vector<vector<int>> normalID;

    void loadObj(const string &filepath);
    const string toString();
};
