#include "ForwardRenderingLibrary.hpp"
namespace RenderingPipeline
{
    vector<PixcelInputStandard> VertOuts2PixcelIns(vector<VertOutputStandard> outs)
    {
        vector<PixcelInputStandard> pis;
        for (VertOutputStandard v : outs)
        {
            pis.push_back(PixcelInputStandard(v));
        }
        return pis;
    }
    namespace Forward
    {

        /// @brief 3Dモデルをレンダーターゲットに描画する
        /// @param model 描画する3Dモデル
        /// @param in カメラやモデル座標情報
        /// @param rt 描画出力先
        /// @param vert 頂点シェーダー関数ポインタ
        /// @param pixcel ピクセルシェーダー関数ポインタ
        void SimpleForwardDrawModelWireframe(Model &model,
                                             const VertInputStandard &in,
                                             RenderTarget &rt,
                                             const VertOutputStandard (*vert)(const VertInputStandard &in),
                                             const PixcelOutputStandard (*pixcel)(const PixcelInputStandard &in))
        {
            VertInputStandard vin = in;
            vin.screenSize = rt.getScreenSize();

            model.transformVerts(vin, vert);
            // 各面についてFor
            for (size_t faceIndex = 0; faceIndex < model.facesID.size(); faceIndex++)
            {
                const vector<int> face = model.facesID[faceIndex];

                // 面を構成する各頂点IDについてFor
                vector<VertOutputStandard> outs;
                for (size_t vertIndex = 0; vertIndex < face.size(); vertIndex++)
                {
                    VertOutputStandard out = model.transformed[face[vertIndex]];
                    if (out.positionVS.z() > 0) // 深度が正＝カメラの正面に映ってるなら
                        outs.push_back(out);    // 描画待ち配列に追加
                }
                // 面を一つ描画
                SimpleForwardFillPolygon(VertOuts2PixcelIns(outs), rt, *pixcel);
                SimpleForwardDrawPolygonLine(VertOuts2PixcelIns(outs), rt, *pixcel);
            }
        }

        void SimpleForwardDrawLine(const PixcelInputStandard &start,
                                   const PixcelInputStandard &end,
                                   RenderTarget &rt,
                                   const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in))
        {
            Vector2i startI = start.positionSS.head<2>().cast<int>();
            Vector2i current = startI;
            Vector2i endI = end.positionSS.head<2>().cast<int>();

            float distance = (startI - endI).norm();

            Vector2i d = (endI - current).cwiseAbs();
            Vector2i s = Vector2i((current.x() < endI.x()) ? 1 : -1, (current.y() < endI.y()) ? 1 : -1);
            int err = d.x() - d.y();

            while (true)
            {
                float ratio = (current - endI).norm() / distance;
                PixcelInputStandard draw = PixcelInputStandard::lerp(start, end, ratio);
                rt.PaintPixel(current.x(), current.y(), pixcel(draw).color);
                // 現在の位置にピクセルを描画

                if (current.x() == endI.x() && current.y() == endI.y())
                    break;

                int e2 = err * 2;

                if (e2 > -d.y())
                {
                    err -= d.y();
                    current.x() += s.x();
                }

                if (e2 < d.x())
                {
                    err += d.x();
                    current.y() += s.y();
                }
            }
        }

        void SimpleForwardDrawPolygonLine(const vector<PixcelInputStandard> &points,
                                          RenderTarget &rt,
                                          const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in))
        {
            if (points.size() >= 2)
            {
                SimpleForwardDrawLine(points[0], points[points.size() - 1], rt, pixcel); // 先頭と末尾の接続
                for (size_t i = 0; i < points.size() - 1; i++)
                {
                    SimpleForwardDrawLine(points[i], points[i + 1], rt, pixcel);
                }
            }
        }
        void SimpleForwardFillPolygon(const vector<PixcelInputStandard> &points,
                                      RenderTarget &rt,
                                      const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in))
        {
            // BB
            int minX = INT_MAX, minY = INT_MAX, maxX = INT_MIN, maxY = INT_MIN;
            for (const auto &point : points)
            {
                minX = min((float)minX, point.positionSS.x());
                minY = min((float)minY, point.positionSS.y());
                maxX = max((float)maxX, point.positionSS.x());
                maxY = max((float)maxY, point.positionSS.y());
            }

            for (int y = minY; y <= maxY; ++y)
            {
                vector<int> intersections;
                for (size_t i = 0; i < points.size(); ++i)
                {
                    const auto &p1 = points[i];
                    const auto &p2 = points[(i + 1) % points.size()];

                    // 頂点p1とp2がスキャンラインyに交差するなら
                    if ((p1.positionSS.y() > y && p2.positionSS.y() <= y) || (p1.positionSS.y() <= y && p2.positionSS.y() > y))
                    {
                        // 線分がスキャンラインと交差するなら
                        int xIntersection = p1.positionSS.x() + (y - p1.positionSS.y()) * (p2.positionSS.x() - p1.positionSS.x()) / (p2.positionSS.y() - p1.positionSS.y());
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
                        if (points.size() <= 2)
                            continue;
                        Vector3f uvw = computeBarycentricCoordinates(points[0].positionSS.head<2>(),
                                                                     points[1].positionSS.head<2>(),
                                                                     points[2].positionSS.head<2>(),
                                                                     Vector2f(x, y));
                        PixcelInputStandard draw = PixcelInputStandard::barycentricLerp(
                            points[0], points[1], points[2], uvw.x(), uvw.y(), uvw.z());
                        rt.PaintPixel(x, y, pixcel(draw).color);
                    }
                }
            }
        }
    }
}