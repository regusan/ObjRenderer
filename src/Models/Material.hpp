#pragma once

#include <filesystem>
#include <map>
#include <iostream>
#include <fstream>
#include <optional>
#include "../header/EigenHeader.hpp"
#include "../Rendering/RenderTarget.hpp"
#include "../Engine/AssetSubSystem.hpp"
using namespace std;
class Material
{
private:
    /* data */
public:
    enum class ShaderModel
    {
        Phoneg,
        PBR
    };

    // 共用パラメータ
    Vector3f diffuse = Vector3f(.8, .8, .8);
    Vector3f emission = Vector3f(0, 0, 0);
    float alpha = 1;
    int illuminationModel = 2;
    ShaderModel shaderModel = ShaderModel::Phoneg;

    // 古典パラメータ
    float specularShapness = 30;
    Vector3f ambientReflection = Vector3f(1, 0, 1);
    Vector3f specular = Vector3f(.3, .3, .3);

    // PBRパラメータ
    float pbrRoughness = 0;
    float pbrMetalic = 0;

    /*各種テクスチャ*/
    shared_ptr<RenderTarget> diffuseMap;
    shared_ptr<RenderTarget> ambientMap;
    shared_ptr<RenderTarget> alphaMap;
    shared_ptr<RenderTarget> bumpMap;
    shared_ptr<RenderTarget> normalMap;

    /// @brief マテリアルをMap<名前、オブジェクト>で読み取り
    /// @param path
    /// @return マテリアル(Map<名前、オブジェクト>)
    static map<string, Material> ReadAllMaterialsFromMTL(string path);
    friend std::ostream &operator<<(std::ostream &os, const Material &mt);

    Material(/* args */);
    ~Material();
};
