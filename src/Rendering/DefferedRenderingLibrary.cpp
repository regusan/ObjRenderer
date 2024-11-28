#include "DefferedRenderingLibrary.hpp"
namespace RenderingPipeline
{
    namespace Deffered
    {
        /// @brief 3Dモデルをレンダーターゲットに描画する
        /// @param model 描画する3Dモデル
        /// @param in カメラやモデル座標情報
        /// @param rt 描画出力先
        void DefferedDrawModel(
            Model &model,
            const VertInputStandard &in,
            GBuffers &gb,
            const VertOutputStandard (*vert)(const VertInputStandard &in),
            const PixcelOutputStandard (*pixcel)(const PixcelInputStandard &in))
        {
            VertInputStandard nonparallel_vin = in;
            nonparallel_vin.screenSize = gb.beauty.getScreenSize();
            bool backfaceCulling = true;

//  各面についてFor
#pragma omp parallel for
            for (size_t faceIndex = 0; faceIndex < model.facesID.size(); faceIndex++)
            {
                VertInputStandard vin = nonparallel_vin;
                const vector<int> face = model.facesID[faceIndex];
                const vector<int> facenorm = model.normalID[faceIndex];
                const vector<int> faceuv = model.uvID[faceIndex];

                // 面を構成する各頂点IDについてFor
                vector<VertOutputStandard> outs;
                for (size_t vertIndex = 0; vertIndex < face.size(); vertIndex++)
                {
                    vin.position = model.verts[face[vertIndex]];
                    vin.normal = model.vertNormals[facenorm[vertIndex]];
                    vin.uv = model.uv[faceuv[vertIndex]];
                    VertOutputStandard out = vert(vin); // 頂点シェーダーでクリップ座標系に変換

                    if (out.positionVS.z() > 0) // 深度が正＝カメラの正面に映ってるなら
                        outs.push_back(out);    // 描画待ち配列に追加
                }
                if (outs.size() >= 3)
                {
                    Vector3f norm = ComputeFaceNormal(outs[0].positionVS.head<3>(), outs[1].positionVS.head<3>(), outs[2].positionVS.head<3>());
                    if (norm.z() > -0.1 || !backfaceCulling)
                    {
                        // 面を一つ描画
                        SimpleDefferedFillPolygon(RenderingPipeline::VertOuts2PixcelIns(outs), gb, *pixcel);
                    }
                }
            }
        }
        void SimpleDefferedFillPolygon(
            const vector<PixcelInputStandard> &points,
            GBuffers &gb,
            const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in))
        {
            // BB
            int minX = INT_MAX, minY = INT_MAX, maxX = INT_MIN, maxY = INT_MIN;
            for (const auto &point : points)
            {
                minX = min((float)minX, point.positionNDC.x());
                minY = min((float)minY, point.positionNDC.y());
                maxX = max((float)maxX, point.positionNDC.x());
                maxY = max((float)maxY, point.positionNDC.y());
            }

            for (int y = minY; y <= maxY; ++y)
            {
                vector<int> intersections;
                for (size_t i = 0; i < points.size(); ++i)
                {
                    const auto &p1 = points[i];
                    const auto &p2 = points[(i + 1) % points.size()];

                    // 頂点p1とp2がスキャンラインyに交差するなら
                    if ((p1.positionNDC.y() > y && p2.positionNDC.y() <= y) || (p1.positionNDC.y() <= y && p2.positionNDC.y() > y))
                    {
                        // 線分がスキャンラインと交差するなら
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
                        if (points.size() <= 2)
                            continue;
                        Vector3f uvw = computeBarycentricCoordinates(points[0].positionNDC.head<2>(),
                                                                     points[1].positionNDC.head<2>(),
                                                                     points[2].positionNDC.head<2>(),
                                                                     Vector2f(x, y));
                        PixcelInputStandard draw = PixcelInputStandard::barycentricLerp(
                            points[0], points[1], points[2], uvw.x(), uvw.y(), uvw.z());
                        PixcelOutputStandard out = pixcel(draw);
                        float depth = draw.positionVS.z();
                        if (depth > gb.depth.SampleColor(x, y).x()) // 深度チェック
                            continue;
                        gb.forward.PaintPixel(x, y, out.color);
                        gb.depth.PaintPixel(x, y, Vector3f(draw.positionVS.head<3>().z(), draw.positionVS.head<3>().z(), draw.positionVS.head<3>().z()));
                        gb.diffuse.PaintPixel(x, y, draw.vertColor.head<3>());

                        gb.positionWS.PaintPixel(x, y, draw.positionWS.head<3>());
                        gb.positionVS.PaintPixel(x, y, draw.positionVS.head<3>());

                        gb.normalVS.PaintPixel(x, y, draw.normalVS.head<3>());
                        gb.normalWS.PaintPixel(x, y, draw.normalWS.head<3>());

                        gb.uv.PaintPixel(x, y, Vector3f(draw.uv.x(), draw.uv.y(), 0));
                    }
                }
            }
        }
    }
}
