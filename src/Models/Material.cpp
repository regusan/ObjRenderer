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
    map<string, Material> materials = {};
    std::ifstream file(pathstring);

    if (!file.is_open())
    {
        std::cerr << "Failed to open MTL file.Use default material: " << pathstring << std::endl;
        materials["Default"] = Material();
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
        else if (key == "Ke")
        {
            iss >> currentMaterial.emission.x() >> currentMaterial.emission.y() >> currentMaterial.emission.z();
        }
        else if (key == "d" || key == "Tr")
        {
            iss >> currentMaterial.alpha;
        }
        else if (key == "illum")
        {
            iss >> currentMaterial.illuminationModel;
        }
        // PBR系
        else if (key == "Pr")
        {
            currentMaterial.shaderModel = ShaderModel::PBR;
            iss >> currentMaterial.pbrRoughness;
        }
        else if (key == "Pm")
        {
            currentMaterial.shaderModel = ShaderModel::PBR;
            iss >> currentMaterial.pbrMetalic;
        }
        // テクスチャ
        else if (key == "map_Kd")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.diffuseMap = AssetSubSystem::getInstance().textureManager.LoadAsset(dir / texturePath);
        }
        else if (key == "map_Ka")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.ambientMap = AssetSubSystem::getInstance().textureManager.LoadAsset(dir / texturePath);
        }
        else if (key == "map_d")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.alphaMap = AssetSubSystem::getInstance().textureManager.LoadAsset(dir / texturePath);
        }
        else if (key == "bump" || key == "map_Bump" || key == "map_bump")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.bumpMap = AssetSubSystem::getInstance().textureManager.LoadAsset(dir / texturePath);
        }
        else if (key == "norm", key == "norm_map")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.normalMap = AssetSubSystem::getInstance().textureManager.LoadAsset(dir / texturePath);
        }
        else if (key == "map_Pm")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.metalicMap = AssetSubSystem::getInstance().textureManager.LoadAsset(dir / texturePath);
        }
        else if (key == "map_Pr")
        {
            string texturePath;
            iss >> texturePath;
            currentMaterial.roughnessMap = AssetSubSystem::getInstance().textureManager.LoadAsset(dir / texturePath);
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

    os << "Shader Model: " << ((mt.shaderModel == Material::ShaderModel::Phoneg) ? "Phoneg" : "PBR") << std::endl;
    os << "Ambient Reflection: " << mt.ambientReflection.x() << ", "
       << mt.ambientReflection.y() << ", " << mt.ambientReflection.z() << std::endl;

    os << "Diffuse: " << mt.diffuse.x() << ", "
       << mt.diffuse.y() << ", " << mt.diffuse.z() << std::endl;

    os << "Specular: " << mt.specular.x() << ", "
       << mt.specular.y() << ", " << mt.specular.z() << std::endl;

    os << "Specular Shininess: " << mt.specularShapness << std::endl;

    os << "Emission: " << mt.emission.x() << ", "
       << mt.emission.y() << ", " << mt.emission.z() << std::endl;

    os << "Alpha: " << mt.alpha << std::endl;

    os << "Illumination Model: " << mt.illuminationModel << std::endl;

    os << "PBR Roughness: " << mt.pbrRoughness << std::endl;
    os << "PBR Metalness: " << mt.pbrMetalic << std::endl;

    if (mt.diffuseMap)
        os << "Diffuse Map: " << mt.diffuseMap << std::endl;

    if (mt.ambientMap)
        os << "Ambient Map: " << mt.ambientMap << std::endl;

    if (mt.alphaMap)
        os << "Alpha Map: " << mt.alphaMap << std::endl;

    if (mt.bumpMap)
        os << "Bump Map: " << mt.bumpMap << std::endl;

    if (mt.normalMap)
        os << "Normal Map: " << mt.normalMap << std::endl;

    return os;
}