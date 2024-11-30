#include "Material.hpp"

Material::Material(/* args */)
{
}

Material::~Material()
{
}

map<string, Material> Material::ReadAllMaterialsFromMTL(string pathstring)
{
    filesystem::path dir = filesystem::path(pathstring).parent_path();
    map<string, Material> materials;
    std::ifstream file(pathstring);

    if (!file.is_open())
    {
        std::cerr << "Failed to open MTL file: " << pathstring << std::endl;
        return materials;
    }

    Material currentMaterial;
    string currentMaterialName;
    string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        string key;
        iss >> key;

        if (key.empty() || key[0] == '#') // コメントまたは空行をスキップ
            continue;

        if (key == "newmtl")
        {
            if (!currentMaterialName.empty())
            {
                materials[currentMaterialName] = currentMaterial; // 現在のマテリアルを保存
                currentMaterial = Material();                     // 新しいマテリアルオブジェクトを初期化
            }
            iss >> currentMaterialName; // マテリアル名を取得
        }
        else if (key == "Ka")
        {
            iss >> currentMaterial.ambientReflection.x() >> currentMaterial.ambientReflection.y() >> currentMaterial.ambientReflection.z();
        }
        else if (key == "Kd")
        {
            iss >> currentMaterial.diffuse.x() >> currentMaterial.diffuse.y() >> currentMaterial.diffuse.z();
        }
        else if (key == "Ks")
        {
            iss >> currentMaterial.specular.x() >> currentMaterial.specular.y() >> currentMaterial.specular.z();
        }
        else if (key == "Ns")
        {
            iss >> currentMaterial.specularShapness;
        }
        else if (key == "d" || key == "Tr")
        {
            iss >> currentMaterial.alpha;
        }
        else if (key == "illum")
        {
            iss >> currentMaterial.illuminationModel;
        }
        else if (key == "map_Kd")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.diffuseMap = RenderTarget(dir / texturePath); // 適切なテクスチャ読み込み処理を実装
        }
        else if (key == "map_Ka")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.ambientMap = RenderTarget(dir / texturePath);
        }
        else if (key == "map_d")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.alphaMap = RenderTarget(dir / texturePath);
        }
        else if (key == "bump" || key == "map_Bump")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.bumpMap = RenderTarget(dir / texturePath);
        }
        else if (key == "norm")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.normalMap = RenderTarget(dir / texturePath);
        }
    }

    // 最後のマテリアルを保存
    if (!currentMaterialName.empty())
    {
        materials[currentMaterialName] = currentMaterial;
    }

    file.close();
    return materials;
}

std::ostream &operator<<(std::ostream &os, const Material &mt)
{
    os << "Material Information:" << std::endl;

    os << "Ambient Reflection: " << mt.ambientReflection.x() << ", "
       << mt.ambientReflection.y() << ", " << mt.ambientReflection.z() << std::endl;

    os << "Diffuse: " << mt.diffuse.x() << ", "
       << mt.diffuse.y() << ", " << mt.diffuse.z() << std::endl;

    os << "Specular: " << mt.specular.x() << ", "
       << mt.specular.y() << ", " << mt.specular.z() << std::endl;

    os << "Specular Shininess: " << mt.specularShapness << std::endl;
    os << "Alpha: " << mt.alpha << std::endl;
    os << "Illumination Model: " << mt.illuminationModel << std::endl;

    return os;
}
