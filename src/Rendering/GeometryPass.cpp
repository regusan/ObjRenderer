#include "GeometryPass.hpp"
#define PARALLEL_FOR_TRANSFORM
namespace RenderingPipeline
{

    namespace Deffered
    {
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
                vector<VertInputStandard> vins;
                for (size_t vertIndex = 0; vertIndex < face.size(); vertIndex++)
                {
                    vin.position = model.verts[face[vertIndex]];
                    vin.normal = model.vertNormals[facenorm[vertIndex]];
                    vin.uv = model.uv[faceuv[vertIndex]];
                    VertOutputStandard out = vert(vin); // 頂点シェーダーでクリップ座標系に変換
                    outs.push_back(out);                // 描画待ち配列に追加
                    vins.push_back(vin);
                }

                if (outs.size() < 3) // 面以外は処理しない
                    continue;

                // テッセレーション処理開始
                int tessCount = 0;
                // バンプマップがあるときのみ分割
                if (vin.material->bumpMap)
                    tessCount = Tessellation::CalcTessLevel(
                        outs[0].positionSS.head<2>(),
                        outs[1].positionSS.head<2>(),
                        outs[2].positionSS.head<2>(), 200, in.environment.MaxTesselleateCount);
                auto tessVIns = Tessellation::TessellateSpecificArea(vins, tessCount);
                vector<vector<VertOutputStandard>> tessVOuts;
                for (size_t tessIndex = 0; tessIndex < tessVIns.size(); tessIndex++)
                {
                    vector<VertOutputStandard> tmpouts;
                    for (size_t vertIndex = 0; vertIndex < face.size(); vertIndex++)
                        tmpouts.push_back(vert(tessVIns[tessIndex][vertIndex]));
                    tessVOuts.push_back(tmpouts);
                }

                // 分割された各面について
                for (size_t tessedFaceIndex = 0; tessedFaceIndex < tessVOuts.size(); tessedFaceIndex++)
                {
                    vector<VertOutputStandard> &tessOut = tessVOuts[tessedFaceIndex];
                    Vector3f norm = GeometryMath::ComputeFaceNormal(tessOut[0].positionVS.head<3>(), tessOut[1].positionVS.head<3>(), tessOut[2].positionVS.head<3>());
                    Vector3f centor = (tessOut[0].positionVS + tessOut[1].positionVS + tessOut[2].positionVS).head<3>() / 3;
                    float orientation = norm.dot(centor);
                    bool backfacecull = in.environment.backFaceCullingDirection * orientation >= 0 || !in.environment.backfaceCulling;
                    bool isFront = tessOut[0].positionVS.z() > 0 && tessOut[1].positionVS.z() > 0 && tessOut[2].positionVS.z() > 0;

                    if (backfacecull && isInFrustum(tessOut) && isFront)
                    {
                        // 面を一つ描画
                        SimpleDefferedFillPolygon(RenderingPipeline::VertOuts2PixcelIns(tessOut), gb, *pixcel);
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
            // ディスプレイにフィッティング
            minX = max(minX, 0);
            minY = max(minY, 0);
            maxX = min(maxX, gb.screenSize.x() - 1);
            maxY = min(maxY, gb.screenSize.y() - 1);

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
                    // 線分間で補完するための値
                    Vector3f startUVW = computeBarycentricCoordinates(points[0].positionSS.head<2>(),
                                                                      points[1].positionSS.head<2>(),
                                                                      points[2].positionSS.head<2>(),
                                                                      Vector2f(intersections[i], y));
                    Vector3f endUVW = computeBarycentricCoordinates(points[0].positionSS.head<2>(),
                                                                    points[1].positionSS.head<2>(),
                                                                    points[2].positionSS.head<2>(),
                                                                    Vector2f(intersections[i + 1], y));
                    float invScanXLength = 1.0f / (intersections[i + 1] - intersections[i]);
                    for (int x = intersections[i]; x < intersections[i + 1]; ++x)
                    {
                        if (points.size() <= 2)
                            continue;

                        // 現在のピクセルの重心座標を計算
                        float interpRatio = (float)(x - intersections[i]) * invScanXLength;
                        Vector3f uvw = startUVW * (1.0f - interpRatio) + endUVW * interpRatio;

                        // 早期シェーダー用の補完値を計算
                        Vector4f positionVS = points[0].positionVS * uvw.x() + points[1].positionVS * uvw.y() + points[2].positionVS * uvw.z();
                        float depth = positionVS.z();
                        Vector4f normalVS = points[0].normalVS * uvw.x() + points[1].normalVS * uvw.y() + points[2].normalVS * uvw.z();

                        if (normalVS.z() > 0 && depth < gb.backDepth.SampleColor(x, y).z())
                        {
                            gb.backDepth.PaintPixel(x, y, Vector3f(depth, depth, depth));
                            gb.backPositionVS.PaintPixel(x, y, positionVS.head<3>());
                            gb.backNormalVS.PaintPixel(x, y, normalVS.head<3>());
                        }

                        // 早期シェーダーのための深度チェックに引っかかったら終了
                        if (depth > gb.depth.SampleColor(x, y).x())
                            continue;

                        //  重心座標を元にピクセルの情報を補間
                        PixcelInputStandard draw = PixcelInputStandard::barycentricLerp(
                            points[0], points[1], points[2], uvw.x(), uvw.y(), uvw.z());
                        PixcelOutputStandard out = pixcel(draw);

                        gb.forward.PaintPixel(x, y, out.color);
                        gb.depth.PaintPixel(x, y, Vector3f(depth, depth, depth));

                        gb.diffuse.PaintPixel(x, y, out.diffuse + out.emission);
                        gb.specular.PaintPixel(x, y, out.specular);
                        gb.ORM.PaintPixel(x, y, out.ORM);

                        gb.positionWS.PaintPixel(x, y, draw.positionWS.head<3>());
                        gb.positionVS.PaintPixel(x, y, draw.positionVS.head<3>());

                        gb.normalVS.PaintPixel(x, y, out.normalVS);
                        gb.normalWS.PaintPixel(x, y, out.normalWS);
                    }
                }
            }
        }

    }
    namespace Tessellation
    {

        vector<vector<PixcelInputStandard>> TessellateTriangle(
            const vector<PixcelInputStandard> &points)
        {
            vector<vector<PixcelInputStandard>> retval;
            PixcelInputStandard centor = PixcelInputStandard::barycentricLerp(points[0], points[1], points[2], 1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f);
            for (size_t i = 0; i < points.size(); i++)
            {
                retval.push_back(vector<PixcelInputStandard>{points[i % points.size()], points[(i + 1) % points.size()], centor});
            }
            return retval;
        }
        vector<vector<VertInputStandard>> TessellateTriangle4(
            const vector<VertInputStandard> &points)
        {
            vector<vector<VertInputStandard>> retval;
            constexpr float div3 = 1.0f / 3.0f;
            VertInputStandard mid0_1 = (points[0] + points[1]) * 0.5;
            VertInputStandard mid1_2 = (points[1] + points[2]) * 0.5;
            VertInputStandard mid2_0 = (points[2] + points[0]) * 0.5;
            retval.push_back(vector<VertInputStandard>{mid0_1, mid1_2, mid2_0});
            retval.push_back(vector<VertInputStandard>{points[0], mid0_1, mid2_0});
            retval.push_back(vector<VertInputStandard>{points[1], mid1_2, mid0_1});
            retval.push_back(vector<VertInputStandard>{points[2], mid2_0, mid1_2});
            return retval;
        }
        vector<vector<PixcelInputStandard>> TessellateRecuirsive(const vector<PixcelInputStandard> &points, const int tessellateCount)
        {
            vector<vector<PixcelInputStandard>> first = vector<vector<PixcelInputStandard>>{points};
            auto logs = vector<vector<vector<PixcelInputStandard>>>{first};
            float area = GeometryMath::ComputeTriangleArea(points[0].positionSS.head<2>(), points[1].positionSS.head<2>(), points[2].positionSS.head<2>());

            int N = tessellateCount;
            //  N回分割開始
            for (int i = 0; i < N; i++)
            {
                vector<vector<PixcelInputStandard>> next;
                // もともと入っていた各頂点に対して分割
                for (int i = 0; i < logs.back().size(); i++)
                {
                    vector<vector<PixcelInputStandard>> displaced = TessellateTriangle(logs.back()[i]);
                    for (auto itr : displaced)
                        next.push_back(itr);
                }
                logs.push_back(next);
            }
            return logs.back();
        }
        vector<vector<VertInputStandard>> TessellateSpecificArea(const vector<VertInputStandard> &points, const int tessellateCount)
        {
            vector<vector<VertInputStandard>> first = vector<vector<VertInputStandard>>{points};
            auto logs = vector<vector<vector<VertInputStandard>>>{first};
            for (int i = 0; i < tessellateCount; i++)
            {
                vector<vector<VertInputStandard>> next;
                // もともと入っていた各頂点に対して分割
                for (int i = 0; i < logs.back().size(); i++)
                {
                    vector<vector<VertInputStandard>> displaced = TessellateTriangle4(logs.back()[i]);
                    for (auto itr : displaced)
                        next.push_back(itr);
                }
                logs.push_back(next);
            }
            return logs.back();
        }
    }
    namespace Lighting
    {
        void ExecLightGeometryPass(
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
            for (int faceIndex = 0; faceIndex < model.facesID.size(); faceIndex++)
            {
                if (model.materials[model.materialNames[model.materialID[faceIndex]]].emission.norm() <= 0.0001)
                    continue;
                VertInputStandard vin = nonparallel_vin;
                const vector<int> face = model.facesID[faceIndex];
                const vector<int> facenorm = model.normalID[faceIndex];
                const vector<int> faceuv = model.uvID[faceIndex];
                vin.material = &model.materials[model.materialNames[model.materialID[faceIndex]]];

                // 面を構成する各頂点IDについてFor
                vector<VertOutputStandard> outs;
                Vector4f centorPosOS = Vector4f::Zero();
                for (int vertIndex = 0; vertIndex < face.size(); vertIndex++)
                {
                    constexpr float lightScale = 2.0f;
                    centorPosOS += model.verts[face[vertIndex]];
                    vin.normal = model.vertNormals[facenorm[vertIndex]];
                    vin.position = model.verts[face[vertIndex]] + vin.normal * lightScale;
                    vin.position.w() = 1;
                    vin.uv = model.uv[faceuv[vertIndex]];
                    VertOutputStandard out = vert(vin); // 頂点シェーダーでクリップ座標系に変換
                    outs.push_back(out);                // 描画待ち配列に追加
                }
                centorPosOS /= face.size();
                Vector4f centorPosVS = in.viewMat * in.modelMat * centorPosOS;

                vector<VertOutputStandard> outsLight;
                if (outs.size() >= 3 && vin.material->emission.norm() > 0)
                {
                    VertInputStandard vinLight = nonparallel_vin;
                    for (int vertIndex = 0; vertIndex < face.size(); vertIndex++)
                    {
                        vinLight.normal = model.vertNormals[facenorm[vertIndex]];
                        vinLight.position = model.verts[face[vertIndex]];
                        vinLight.uv = model.uv[faceuv[vertIndex]];
                        VertOutputStandard out = vert(vinLight); // 頂点シェーダーでクリップ座標系に変換
                        outsLight.push_back(out);                // 描画待ち配列に追加
                    }

                    Vector3f norm = GeometryMath::ComputeFaceNormal(outs[0].positionVS.head<3>(), outs[1].positionVS.head<3>(), outs[2].positionVS.head<3>());
                    bool isFront = outs[0].positionVS.z() > 0 && outs[1].positionVS.z() > 0 && outs[2].positionVS.z() > 0;

                    if (isInFrustum(outsLight) && isFront)
                    {
                        // 面を一つ描画
                        FillLightPolygon(RenderingPipeline::VertOuts2PixcelIns(outs), centorPosVS.head<3>(), gb, *pixcel);
                    }
                }
            }
        }

        void FillLightPolygon(
            const vector<PixcelInputStandard> &points,
            Vector3f centorPosVS,
            GBuffers &gb,
            const PixcelOutputStandard (&pixcel)(const PixcelInputStandard &in))
        {
            Vector3f centor = (points[0].positionVS + points[1].positionVS + points[2].positionVS).head<3>() / 3;
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
            // ディスプレイにフィッティング
            minX = max(minX, 0);
            minY = max(minY, 0);
            maxX = min(maxX, gb.screenSize.x() - 1);
            maxY = min(maxY, gb.screenSize.y() - 1);

            for (int y = minY; y <= maxY; ++y)
            {
                vector<int> intersections;
                for (int i = 0; i < points.size(); ++i)
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
                for (int i = 0; i < intersections.size(); i += 2)
                {
                    // 線分間で補完するための値
                    Vector3f startUVW = computeBarycentricCoordinates(points[0].positionSS.head<2>(),
                                                                      points[1].positionSS.head<2>(),
                                                                      points[2].positionSS.head<2>(),
                                                                      Vector2f(intersections[i], y));
                    Vector3f endUVW = computeBarycentricCoordinates(points[0].positionSS.head<2>(),
                                                                    points[1].positionSS.head<2>(),
                                                                    points[2].positionSS.head<2>(),
                                                                    Vector2f(intersections[i + 1], y));
                    float invScanXLength = 1.0f / (intersections[i + 1] - intersections[i]);
                    for (int x = intersections[i]; x < intersections[i + 1]; ++x)
                    {
                        if (points.size() <= 2)
                            continue;

                        // 現在のピクセルの重心座標を計算
                        float interpRatio = (float)(x - intersections[i]) * invScanXLength;
                        Vector3f uvw = startUVW * (1.0f - interpRatio) + endUVW * interpRatio;

                        // 早期シェーダー用の補完値を計算
                        Vector4f positionVS = points[0].positionVS * uvw.x() + points[1].positionVS * uvw.y() + points[2].positionVS * uvw.z();
                        float depth = positionVS.z();
                        Vector4f normalVS = points[0].normalVS * uvw.x() + points[1].normalVS * uvw.y() + points[2].normalVS * uvw.z();

                        // 早期シェーダーのための深度チェックに引っかかったら終了
                        if (depth > gb.depth.SampleColor(x, y).x())
                            continue;

                        //  重心座標を元にピクセルの情報を補間
                        PixcelInputStandard draw = PixcelInputStandard::barycentricLerp(
                            points[0], points[1], points[2], uvw.x(), uvw.y(), uvw.z());
                        PixcelOutputStandard out = pixcel(draw);
                        if (draw.normalVS.dot(positionVS) > 0)
                        {
                            gb.lightBackDepth.PaintPixel(x, y, Vector3f(depth, depth, depth));
                        }
                        else
                        {
                            gb.lightDepth.PaintPixel(x, y, Vector3f(depth, depth, depth));
                            gb.lightPositionVS.PaintPixel(x, y, centorPosVS);
                        }
                        gb.lightDomain.PaintPixel(x, y, gb.lightDomain.SampleColor(x, y).array() + out.diffuse.array() * out.emission.array());
                    }
                }
            }
        }
    }
    namespace Forward
    {
        void ExecWireFramePass(
            Model &model,
            const VertInputStandard &in,
            GBuffers &gb,
            const VertOutputStandard (*vert)(const VertInputStandard &in))
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
                vector<VertInputStandard> vins;

                for (size_t vertIndex = 0; vertIndex < face.size(); vertIndex++)
                {
                    vin.position = model.verts[face[vertIndex]];
                    vin.normal = model.vertNormals[facenorm[vertIndex]];
                    vin.uv = model.uv[faceuv[vertIndex]];
                    vins.push_back(vin);
                    VertOutputStandard out = vert(vin); // 頂点シェーダーでクリップ座標系に変換
                    outs.push_back(out);                // 描画待ち配列に追加
                }
                if (outs.size() < 3) // 面以外は処理しない
                    continue;

                // テッセレーション処理開始
                int tessCount = 0;
                // バンプマップがあるときのみ分割
                if (vin.material->bumpMap)
                    tessCount = Tessellation::CalcTessLevel(
                        outs[0].positionSS.head<2>(),
                        outs[1].positionSS.head<2>(),
                        outs[2].positionSS.head<2>(), 200, in.environment.MaxTesselleateCount);
                auto tessVIns = Tessellation::TessellateSpecificArea(vins, tessCount);
                vector<vector<VertOutputStandard>> tessVOuts;
                for (size_t tessIndex = 0; tessIndex < tessVIns.size(); tessIndex++)
                {
                    vector<VertOutputStandard> tmpouts;
                    for (size_t vertIndex = 0; vertIndex < face.size(); vertIndex++)
                        tmpouts.push_back(vert(tessVIns[tessIndex][vertIndex]));
                    tessVOuts.push_back(tmpouts);
                }

                // 分割された各面について
                for (size_t tessedFaceIndex = 0; tessedFaceIndex < tessVOuts.size(); tessedFaceIndex++)
                {
                    vector<VertOutputStandard> &tessOut = tessVOuts[tessedFaceIndex];

                    Vector3f norm = GeometryMath::ComputeFaceNormal(tessOut[0].positionVS.head<3>(), tessOut[1].positionVS.head<3>(), tessOut[2].positionVS.head<3>());
                    bool backfacecull = in.environment.backFaceCullingDirection * norm.z() >= 0 || !in.environment.backfaceCulling;
                    bool isFront = tessOut[0].positionVS.z() > 0 && tessOut[1].positionVS.z() > 0 && tessOut[2].positionVS.z() > 0;

                    if (backfacecull && isInFrustum(tessOut) && isFront)
                    {
                        for (size_t i = 0; i < tessOut.size() + 1; i++)
                        {
                            gb.beauty.DrawLine(tessOut[i % tessOut.size()].positionSS.head<2>(), tessOut[(i + 1) % tessOut.size()].positionSS.head<2>(), tessOut[0].material->diffuse);
                        }
                    }
                }
            }
        }
    }
}
