#include "Model.hpp"

Model::Model() {}

Model::~Model() {}
void Model::loadObj(const string &filepath)
{
    ifstream file(filepath); // ファイルを開く
    if (!file.is_open())
    {
        throw runtime_error("Could not open file: " + filepath); // ファイルが開けない場合はエラー
    }

    string line;
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
            verts.push_back(Vector4f(x, y, z, 1)); // Eigen::Vector3fに格納
        }
        else if (prefix == "vn")
        {
            // 法線ベクトル (nx, ny, nz)
            float nx, ny, nz;
            ss >> nx >> ny >> nz;
            vertNormals.push_back(Vector4f(nx, ny, nz, 1)); // Eigen::Vector3fに格納
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
                // 頂点インデックス/テクスチャ座標インデックス/法線インデックス
                size_t firstSlash = token.find('/');
                size_t secondSlash = token.rfind('/');

                int vertIndex = 0, uvIndex = 0, normalIndex = 0;

                if (firstSlash != string::npos)
                {
                    string vertPart = token.substr(0, firstSlash);
                    if (!vertPart.empty())              // 空文字列チェック
                        vertIndex = stoi(vertPart) - 1; // 1-based to 0-based
                }
                else if (!token.empty())
                { // トークンが空でないかチェック
                    vertIndex = stoi(token) - 1;
                }

                // テクスチャ座標インデックスの取得
                if (firstSlash != string::npos && secondSlash != firstSlash)
                {
                    string uvPart = token.substr(firstSlash + 1, secondSlash - firstSlash - 1);
                    if (!uvPart.empty()) // 空文字列チェック
                        uvIndex = stoi(uvPart) - 1;
                }

                // 法線インデックスの取得
                if (secondSlash != string::npos)
                {
                    string normalPart = token.substr(secondSlash + 1);
                    if (!normalPart.empty()) // 空文字列チェック
                        normalIndex = stoi(normalPart) - 1;
                }

                faceVerts.push_back(vertIndex);
                faceUVs.push_back(uvIndex);
                faceNormals.push_back(normalIndex);
            }

            // 3頂点で1つの面を作成
            if (faceVerts.size() == 3)
            {
                facesID.push_back(faceVerts);    // 頂点インデックス
                uvID.push_back(faceUVs);         // UVインデックス
                normalID.push_back(faceNormals); // 法線インデックス
            }
        }
        else if (prefix == "f")
        {
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
