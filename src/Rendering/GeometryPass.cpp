#include "GeometryPass.hpp"
#define PARALLEL_FOR_TRANSFORM
namespace RenderingPipeline
{

    namespace Deffered
    {
        /// @brief 3Dモデルをレンダーターゲットに描画する
        /// @param model 描画する3Dモデル
        /// @param in カメラやモデル座標情報
        /// @param rt 描画出力先
        void ExecGeometryPass(
            Model &model,
            const VertInputStandard &in,
            GBuffers &gb,
            const VertOutputStandard (*vert)(const VertInputStandard &in),
            const PixcelOutputStandard (*pixcel)(const PixcelInputStandard &in))
        {
            VertInputStandard nonparallel_vin = in;
            nonparallel_vin.environment.screenSize = gb.beauty.getScreenSize();

//  各面についてFor(並列処理)
#ifdef PARALLEL_FOR_TRANSFORM
#pragma omp parallel for
#endif
            for (size_t faceIndex = 0; faceIndex < model.facesID.size(); faceIndex++)
            {
                VertInputStandard vin = nonparallel_vin;
                const vector<int> face = model.facesID[faceIndex];
                const vector<int> facenorm = model.normalID[faceIndex];
                const vector<int> faceuv = model.uvID[faceIndex];
                vin.material = &model.materials[model.materialNames[model.materialID[faceIndex]]];

                // 面を構成する各頂点IDについてFor
                vector<VertOutputStandard> outs;
                for (size_t vertIndex = 0; vertIndex < face.size(); vertIndex++)
                {
                    vin.position = model.verts[face[vertIndex]];
                    vin.normal = model.vertNormals[facenorm[vertIndex]];
                    vin.uv = model.uv[faceuv[vertIndex]];
                    VertOutputStandard out = vert(vin); // 頂点シェーダーでクリップ座標系に変換
                    outs.push_back(out);                // 描画待ち配列に追加
                }
                if (outs.size() >= 3)
                {
                    Vector3f norm = ComputeFaceNormal(outs[0].positionVS.head<3>(), outs[1].positionVS.head<3>(), outs[2].positionVS.head<3>());
                    if ((norm.z() > -0.1 || !in.environment.backfaceCulling) && isInFrustum(outs))
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
            // BBをディスプレイサイズで初期化
            int minX = gb.screenSize.x(), minY = gb.screenSize.y(), maxX = 0, maxY = 0;
            // BBを計算
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
                        PixcelOutputStandard out = pixcel(draw);
                        float depth = draw.positionVS.z();
                        if (depth > gb.depth.SampleColor(x, y).x()) // 深度チェック
                            continue;
                        gb.forward.PaintPixel(x, y, out.color);
                        gb.depth.PaintPixel(x, y, Vector3f(draw.positionVS.head<3>().z(), draw.positionVS.head<3>().z(), draw.positionVS.head<3>().z()));
                        gb.diffuse.PaintPixel(x, y, out.diffuse);

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
