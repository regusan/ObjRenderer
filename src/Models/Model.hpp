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
class Model : public REngine::RAsset
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

    /// @brief 面毎のマテリアルのID
    vector<int> materialID;
    /// @brief マテリアルIDに対応するマテリアル名
    vector<string> materialNames;

    // マテリアルの登録が何もない時に使用されるマテリアル
    Material defaultMaterial;

    shared_ptr<RenderTarget> VATPos;
    shared_ptr<RenderTarget> VATNormal;

    map<string, Material> materials;
    bool LoadFromFile(const filesystem::path &filepath) override;

    inline Material &GetMaterialFromFaceIndex(const int faceIndex)
    {
        return (materials.empty()) ? defaultMaterial : materials[materialNames[materialID[faceIndex]]];
    }

    inline const Vector4f GetNormalFromFaceIndexAndVertIndex(const int faceIndex, const int vertIndex)
    {
        static const Vector4f defaultNormal = Vector4f(1, 0, 0, 1);
        return (this->vertNormals.empty()) ? defaultNormal : this->vertNormals[(this->normalID[faceIndex])[vertIndex]];
    }
    inline const Vector4f GetPositionFromFaceIndexAndVertIndex(const int faceIndex, const int vertIndex)
    {
        static const Vector4f defaultPosiiton = Vector4f(1, 0, 0, 1);
        return (this->verts.empty()) ? defaultPosiiton : this->verts[(this->facesID[faceIndex])[vertIndex]];
    }
    inline const Vector2f GetUVFromFaceIndexAndVertIndex(const int faceIndex, const int vertIndex)
    {
        static const Vector2f defaultUV = Vector2f(0, 0);
        return (this->uv.empty()) ? defaultUV : this->uv[(this->uvID[faceIndex])[vertIndex]];
    }

    const string toString();
    friend std::ostream &operator<<(std::ostream &os, Model &model)
    {
        os << model.toString() << endl;
        return os;
    }
};
