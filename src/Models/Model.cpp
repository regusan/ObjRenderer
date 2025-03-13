#include "Model.hpp"

Model::Model() {}

Model::~Model() {}
bool Model::LoadFromFile(const filesystem::path &filepath)
{
    this->loadObj(filepath);
    return true;
}

void Model::loadObj(const filesystem::path &filepath)
{
    ifstream file(filepath); // ファイルを開く
    if (!file.is_open())
    {
        throw runtime_error("Could not open file: " + filepath.string()); // ファイルが開けない場合はエラー
    }
    this->loadedFilepath = filepath;
    string line;
    string currentMaterial = "";       // 現在使用中のマテリアル名
    map<string, int> materialNameToID; // マテリアル名→IDのマップ

    while (getline(file, line)) // 1行ずつ読み込む
    {
        istringstream ss(line); // 行をストリームに変換
        string prefix;
        ss >> prefix;

        if (prefix == "v")
        {
            // 頂点 (x, y, z)
            float x, y, z;
            ss >> x >> y >> z;
            verts.push_back(Vector4f(x, y, z, 1)); // Eigen::Vector4fに格納
        }
        else if (prefix == "vn")
        {
            // 法線ベクトル (nx, ny, nz)
            float nx, ny, nz;
            ss >> nx >> ny >> nz;
            vertNormals.push_back(Vector4f(nx, ny, nz, 1)); // Eigen::Vector4fに格納
        }
        else if (prefix == "vt")
        {
            // UV座標 (u, v)
            float u, v;
            ss >> u >> v;
            uv.push_back(Vector2f(u, v)); // Eigen::Vector2fに格納
        }
        else if (prefix == "f")
        {
            // 面のインデックス (頂点番号/テクスチャ座標番号/法線番号)
            string token;
            vector<int> faceVerts, faceUVs, faceNormals;
            while (ss >> token)
            {
                size_t firstSlash = token.find('/');
                size_t secondSlash = token.rfind('/');

                int vertIndex = 0, uvIndex = 0, normalIndex = 0;

                if (firstSlash != string::npos)
                {
                    string vertPart = token.substr(0, firstSlash);
                    if (!vertPart.empty())
                        vertIndex = stoi(vertPart) - 1; // 0基準
                }
                else if (!token.empty())
                {
                    vertIndex = stoi(token) - 1;
                }

                if (firstSlash != string::npos && secondSlash != firstSlash)
                {
                    string uvPart = token.substr(firstSlash + 1, secondSlash - firstSlash - 1);
                    if (!uvPart.empty())
                        uvIndex = stoi(uvPart) - 1;
                }

                if (secondSlash != string::npos)
                {
                    string normalPart = token.substr(secondSlash + 1);
                    if (!normalPart.empty())
                        normalIndex = stoi(normalPart) - 1;
                }

                faceVerts.push_back(vertIndex);
                faceUVs.push_back(uvIndex);
                faceNormals.push_back(normalIndex);
            }

            if (faceVerts.size() == 3)
            {
                facesID.push_back(faceVerts);
                uvID.push_back(faceUVs);
                normalID.push_back(faceNormals);
                if (!currentMaterial.empty())
                {
                    materialID.push_back(materialNameToID[currentMaterial]); // マテリアルIDを追加
                }
            }
        }
        else if (prefix == "mtllib")
        {
            // マテリアルライブラリの読み込み
            string mtlFilename;
            ss >> mtlFilename;
            filesystem::path mtlFilePath = filepath.parent_path() / mtlFilename;

            // マテリアルをロードしてmaterialsマップに格納
            auto loadedMaterials = Material::ReadAllMaterialsFromMTL(mtlFilePath.string());
            for (const auto &entry : loadedMaterials)
            {
                int id = materials.size();             // 新しいマテリアルID
                materials[entry.first] = entry.second; // 名前でマッピング
                materialNameToID[entry.first] = id;    // 名前→IDのマッピング
                materialNames.push_back(entry.first);  // マテリアル名リストに追加
            }
        }
        else if (prefix == "usemtl")
        {
            // 使用するマテリアルを設定
            ss >> currentMaterial;
        }
    }
    file.close(); // ファイルを閉じる
}

const string Model::toString()
{
    stringstream ss;

    // 頂点 (verts)
    ss << "Vertices:\n";
    for (const auto &v : verts)
    {
        ss << v.x() << " " << v.y() << " " << v.z() << "\n"; // x, y, zを出力
    }

    // 法線 (vertNormals)
    ss << "\nVertex Normals:\n";
    for (const auto &vn : vertNormals)
    {
        ss << vn.x() << " " << vn.y() << " " << vn.z() << "\n"; // 法線ベクトルを出力
    }

    // UV座標 (uv)
    ss << "\nUV Coordinates:\n";
    for (const auto &u : uv)
    {
        ss << u.x() << " " << u.y() << "\n"; // UV座標を出力
    }

    // 面のインデックス (facesID)
    ss << "\nFaces:\n";
    for (const auto &face : facesID)
    {
        ss << face[0] << " " << face[1] << " " << face[2] << "\n"; // 各面を出力
    }

    return ss.str(); // 文字列を返す
}
