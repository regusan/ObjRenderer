

#include "PrimitiveMeshData.hpp"

namespace REngine
{
    std::shared_ptr<Model> PrimitiveMeshData::GenerateUnitBox()
    {
        auto model = std::make_shared<Model>();

        // 頂点データ
        std::vector<Vector4f> verts = {
            {-0.5f, -0.5f, -0.5f, 1.0f},
            {0.5f, -0.5f, -0.5f, 1.0f},
            {0.5f, 0.5f, -0.5f, 1.0f},
            {-0.5f, 0.5f, -0.5f, 1.0f},
            {-0.5f, -0.5f, 0.5f, 1.0f},
            {0.5f, -0.5f, 0.5f, 1.0f},
            {0.5f, 0.5f, 0.5f, 1.0f},
            {-0.5f, 0.5f, 0.5f, 1.0f}};

        model->verts = verts;

        model->facesID = {{0, 1, 2, 3},  // 前面
                          {1, 5, 6, 2},  // 右面
                          {5, 4, 7, 6},  // 背面
                          {4, 0, 3, 7},  // 左面
                          {3, 2, 6, 7},  // 上面
                          {4, 5, 1, 0}}; // 底面

        return model;
    }
    std::shared_ptr<Model> PrimitiveMeshData::GenerateUnitSphere(int subdivisionLevel)
    {
        auto model = std::make_shared<Model>();

        // 正二十面体の初期頂点データ
        float t = (1.0 + std::sqrt(5.0)) / 2.0; // 黄金比
        std::vector<Vector4f> verts = {
            {-1, t, 0, 1}, {1, t, 0, 1}, {-1, -t, 0, 1}, {1, -t, 0, 1}, {0, -1, t, 1}, {0, 1, t, 1}, {0, -1, -t, 1}, {0, 1, -t, 1}, {t, 0, -1, 1}, {t, 0, 1, 1}, {-t, 0, -1, 1}, {-t, 0, 1, 1}};

        // 正二十面体の初期面データ（インデックス）
        std::vector<std::vector<int>> facesID = {
            {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11}, {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8}, {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9}, {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}};

        // 球面への変換
        auto normalizeVertex = [](Vector4f &v)
        {
            Vector3f normalized = v.head<3>().normalized(); // 法線化
            v = Vector4f(normalized.x(), normalized.y(), normalized.z(), 1.0f);
        };

        for (auto &v : verts)
        {
            normalizeVertex(v);
        }

        // 細分化（N回）
        for (int i = 0; i < subdivisionLevel; ++i)
        {
            std::map<std::pair<int, int>, int> middlePointIndexCache;
            auto getMiddlePoint = [&](int p1, int p2) -> int
            {
                std::pair<int, int> key = (p1 < p2) ? std::make_pair(p1, p2) : std::make_pair(p2, p1);
                if (middlePointIndexCache.count(key))
                    return middlePointIndexCache[key];

                Vector4f middle = (verts[p1] + verts[p2]) / 2.0f;
                normalizeVertex(middle);
                verts.push_back(middle);
                int index = verts.size() - 1;
                middlePointIndexCache[key] = index;
                return index;
            };

            std::vector<std::vector<int>> newFaces;
            for (auto &face : facesID)
            {
                int a = getMiddlePoint(face[0], face[1]);
                int b = getMiddlePoint(face[1], face[2]);
                int c = getMiddlePoint(face[2], face[0]);

                newFaces.push_back({face[0], a, c});
                newFaces.push_back({face[1], b, a});
                newFaces.push_back({face[2], c, b});
                newFaces.push_back({a, b, c});
            }
            facesID = newFaces;
        }

        // モデルにデータをセット
        model->verts = verts;
        model->facesID = facesID;

        return model;
    }

    std::shared_ptr<Model> PrimitiveMeshData::GeneratePlane(float width, float height)
    {
        auto model = std::make_shared<Model>();

        model->verts = {{-width / 2, 0, -height / 2, 1}, {width / 2, 0, -height / 2, 1}, {width / 2, 0, height / 2, 1}, {-width / 2, 0, height / 2, 1}};
        model->facesID = {{0, 1, 2}, {2, 3, 0}};

        return model;
    }
}

static std::shared_ptr<Model> GenerateLine(Vector3f start, Vector3f end)
{
    auto model = std::make_shared<Model>();

    // 頂点データ（線の開始点と終了点）
    model->verts = {
        Vector4f(start.x(), start.y(), start.z(), 1.0f),
        Vector4f(end.x(), end.y(), end.z(), 1.0f)};

    // 線のインデックス
    model->facesID = {{0, 1}}; // Lineとして扱うため、2点のみ

    return model;
}
