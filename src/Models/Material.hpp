#pragma once

#include <filesystem>
#include <map>
#include <iostream>
#include <fstream>
#include <optional>
#include "../header/EigenHeader.hpp"
#include "../Rendering/RenderTarget.hpp"
using namespace std;
class Material
{
private:
    /* data */
public:
    Vector3f ambientReflection;
    Vector3f diffuse;
    Vector3f specular;
    float specularShapness;
    float alpha;
    int illuminationModel;
    /*各種テクスチャ*/
    optional<RenderTarget> diffuseMap;
    optional<RenderTarget> ambientMap;
    optional<RenderTarget> alphaMap;
    optional<RenderTarget> bumpMap;
    optional<RenderTarget> normalMap;

    /// @brief マテリアルをMap<名前、オブジェクト>で読み取り
    /// @param path
    /// @return マテリアル(Map<名前、オブジェクト>)
    static map<string, Material> ReadAllMaterialsFromMTL(string path);
    friend std::ostream &operator<<(std::ostream &os, const Material &mt);

    Material(/* args */);
    ~Material();
};
