#pragma once

#include <filesystem>
#include <map>
#include <iostream>
#include <fstream>
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
    RenderTarget diffuseMap;
    RenderTarget ambientMap;
    RenderTarget alphaMap;
    RenderTarget bumpMap;
    RenderTarget normalMap;

    static map<string, Material> ReadAllMaterialsFromMTL(string path);
    friend std::ostream &operator<<(std::ostream &os, const Material &mt);

    Material(/* args */);
    ~Material();
};
