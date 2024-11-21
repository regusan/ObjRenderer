#include "RenderingLibrary.hpp"

inline Vector4f clip2Device(const Vector4f &clippos, const Vector2i &screenSize)
{
    return Vector4f((clippos.x() + 1) * 0.5 * screenSize.x(),
                    (clippos.y() + 1) * 0.5 * screenSize.y(),
                    clippos.z(),
                    1);
}

vector<PixcelInputStandard> VertOuts2PixcelIns(vector<VertOutputStandard> outs)
{
    vector<PixcelInputStandard> pis;
    for (VertOutputStandard v : outs)
    {
        pis.push_back(v.toPixcelInput());
    }
    return pis;
}

/// @brief 3Dモデルをレンダーターゲットに描画する
/// @param model 描画する3Dモデル
/// @param in カメラやモデル座標情報
/// @param rt 描画出力先
void DrawModelWireframe(const Model &model,
                        const VertInputStandard &in,
                        RenderTarget &rt,
                        const VertOutputStandard (*vert)(const VertInputStandard &in),
                        const PixcelOutputStandard (*pixcel)(const PixcelInputStandard &in))
{
    // 各面についてFor
    for (const vector<int> &face : model.facesID)
    {
        // 面を構成する各頂点IDについてFor
        vector<VertOutputStandard> outs;
        for (const int &vertID : face)
        {
            VertInputStandard vin = in;
            Vector3f v = model.verts[vertID]; // 頂点IDから頂点座標取得

            vin.position = Vector4f(v.x(), v.y(), v.z(), 1);
            VertOutputStandard out = vert(vin);                   // 頂点シェーダーでクリップ座標系に変換
            out.faceColor = model.vertNormals[vertID].cwiseAbs(); // 頂点IDから法線取得
            if (out.positionDS.z() > 0)                           // 深度が正＝カメラの正面に映ってるなら
            {
                // クリップ座標系→デバイス座標系
                out.positionDS = clip2Device(out.positionDS, rt.getScreenSize());
                // 描画待ち配列に追加
                outs.push_back(out);
            }
        }
        Vector3f color = Vector3f(255, 0, 0);
        if (!outs.empty())
            color = outs[0].faceColor * 255;
        // 面を一つ描画
        DrawPolygonLine(VertOuts2PixcelIns(outs), rt, *pixcel);
        if (outs.size() >= 2)
        {
            DrawLine(outs[0].toPixcelInput(), outs[1].toPixcelInput(), rt, *pixcel);
        }
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