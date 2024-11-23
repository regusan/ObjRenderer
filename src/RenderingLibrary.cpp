#include "RenderingLibrary.hpp"

vector<PixcelInputStandard> VertOuts2PixcelIns(vector<VertOutputStandard> outs)
{
    vector<PixcelInputStandard> pis;
    for (VertOutputStandard v : outs)
    {
        pis.push_back(PixcelInputStandard(v));
    }
    return pis;
}

/// @brief 3Dモデルをレンダーターゲットに描画する
/// @param model 描画する3Dモデル
/// @param in カメラやモデル座標情報
/// @param rt 描画出力先
/// @param vert 頂点シェーダー関数ポインタ
/// @param pixcel ピクセルシェーダー関数ポインタ
void DrawModelWireframe(const Model &model,
                        const VertInputStandard &in,
                        RenderTarget &rt,
                        const VertOutputStandard (*vert)(const VertInputStandard &in),
                        const PixcelOutputStandard (*pixcel)(const PixcelInputStandard &in))
{
    VertInputStandard vin = in;
    vin.screenSize = rt.getScreenSize();
    // 各面についてFor
    for (size_t faceIndex = 0; faceIndex < model.facesID.size(); faceIndex++)
    {
        const vector<int> face = model.facesID[faceIndex];
        const vector<int> normals = model.normalID[faceIndex];

        // 面を構成する各頂点IDについてFor
        vector<VertOutputStandard> outs;
        for (size_t vertIndex = 0; vertIndex < face.size(); vertIndex++)
        {
            vin.position = model.verts[face[vertIndex]];
            vin.normal = model.vertNormals[face[vertIndex]];
            VertOutputStandard out = vert(vin); // 頂点シェーダーでクリップ座標系に変換
            if (out.positionVS.z() > 0)         // 深度が正＝カメラの正面に映ってるなら
                outs.push_back(out);            // 描画待ち配列に追加
        }
        // 面を一つ描画
        DrawPolygonLine(VertOuts2PixcelIns(outs), rt, *pixcel);
        FillPolygon(VertOuts2PixcelIns(outs), rt, *pixcel);
    }
}

void DrawLine(const PixcelInputStandard &start,
              const PixcelInputStandard &end,
              RenderTarget &rt,
              const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in))
{
    Vector2i startI = start.positionNDC.head<2>().cast<int>();
    Vector2i endI = end.positionNDC.head<2>().cast<int>();
    Vector2i d = (endI - startI).cwiseAbs();
    Vector2i s = Vector2i((startI.x() < endI.x()) ? 1 : -1, (startI.y() < endI.y()) ? 1 : -1);
    int err = d.x() - d.y();
    PixcelInputStandard draw = start;
    while (true)
    {
        rt.PaintPixel(startI.x(), startI.y(), pixcel(draw).color);
        // 現在の位置にピクセルを描画

        if (startI.x() == endI.x() && startI.y() == endI.y())
            break;

        int e2 = err * 2;

        if (e2 > -d.y())
        {
            err -= d.y();
            startI.x() += s.x();
        }

        if (e2 < d.x())
        {
            err += d.x();
            startI.y() += s.y();
        }
    }
}

void DrawPolygonLine(const vector<PixcelInputStandard> &points,
                     RenderTarget &rt,
                     const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in))
{
    if (points.size() >= 2)
    {
        DrawLine(points[0], points[points.size() - 1], rt, pixcel); // 先頭と末尾の接続
        for (size_t i = 0; i < points.size() - 1; i++)
        {
            DrawLine(points[i], points[i + 1], rt, pixcel);
        }
    }
}
void FillPolygon(const vector<PixcelInputStandard> &points,
                 RenderTarget &rt,
                 const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in))
{
    // まず、ポリゴンのバウンディングボックスを計算
    int minX = INT_MAX, minY = INT_MAX, maxX = INT_MIN, maxY = INT_MIN;
    for (const auto &point : points)
    {
        minX = min((float)minX, point.positionNDC.x());
        minY = min((float)minY, point.positionNDC.y());
        maxX = max((float)maxX, point.positionNDC.x());
        maxY = max((float)maxY, point.positionNDC.y());
    }

    // スキャンラインアルゴリズム
    for (int y = minY; y <= maxY; ++y)
    {
        vector<int> intersections;
        // ポリゴンのエッジをスキャンして、y座標に交差するx座標を計算
        for (size_t i = 0; i < points.size(); ++i)
        {
            const auto &p1 = points[i];
            const auto &p2 = points[(i + 1) % points.size()];

            // 頂点p1とp2がスキャンラインyに交差するかどうかを調べる
            if ((p1.positionNDC.y() > y && p2.positionNDC.y() <= y) || (p1.positionNDC.y() <= y && p2.positionNDC.y() > y))
            {
                // 線分がスキャンラインと交差する場合
                int xIntersection = p1.positionNDC.x() + (y - p1.positionNDC.y()) * (p2.positionNDC.x() - p1.positionNDC.x()) / (p2.positionNDC.y() - p1.positionNDC.y());
                intersections.push_back(xIntersection);
            }
        }

        // x座標で交差点をソート
        std::sort(intersections.begin(), intersections.end());

        // 塗りつぶす部分を設定（交差点でペアになるx座標間を塗りつぶす）
        for (size_t i = 0; i < intersections.size(); i += 2)
        {
            for (int x = intersections[i]; x < intersections[i + 1]; ++x)
            {
                // その座標にピクセルを描画
                PixcelInputStandard draw = points[0]; // 任意の点を設定（この場合、最初の点を使って色設定）
                rt.PaintPixel(x, y, pixcel(draw).color);
            }
        }
    }
}
