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
            if (out.positionPS.z() > 0)         // 深度が正＝カメラの正面に映ってるなら
                outs.push_back(out);            // 描画待ち配列に追加
        }
        // 面を一つ描画
        DrawPolygonLine(VertOuts2PixcelIns(outs), rt, *pixcel);
    }
}

void DrawLine(const PixcelInputStandard &start,
              const PixcelInputStandard &end,
              RenderTarget &rt,
              const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in))
{
    Vector2i startI = start.positionDS.head<2>().cast<int>();
    Vector2i endI = end.positionDS.head<2>().cast<int>();
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