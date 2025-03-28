#include "GeometryPass.hpp"
#define PARALLEL_FOR_TRANSFORM
namespace RenderingPipeline
{
    Vector3f EncodeVAT(shared_ptr<Texture2D> vat, int index, float time)
    {
        if (vat)
        {
            Vector3f encorded0 = vat->SampleColor(index, fmod(time, vat->getScreenSize().y())) * 2 - Vector3f::Ones();
            Vector3f encorded1 = vat->SampleColor(index, fmod(time + 1, vat->getScreenSize().y())) * 2 - Vector3f::Ones();
            Vector3f lerped = StandardMath::lerp<Vector3f>(fmod(time, 1), encorded0, encorded1);
            return Vector3f(lerped.x(), lerped.z(), lerped.y()); // z-upからy-upに変換
        }
        else
        {
            return Vector3f::Zero();
        }
    }
    void ApplyVAT(VertInputStandard &vin, const int vertexId, const float time, const shared_ptr<Texture2D> &vatPos, const shared_ptr<Texture2D> &vatNormal)
    {
        if (vatPos)
        {
            Vector3f pos3f = EncodeVAT(vatPos, vertexId, time);
            vin.position += Vector4f(pos3f.x(), pos3f.y(), pos3f.z(), 0);
        }

        if (vatNormal)
        {
            Vector3f norm3f = EncodeVAT(vatNormal, vertexId, time);
            vin.normal = Vector4f(norm3f.x(), norm3f.y(), norm3f.z(), 1);
        }
    }
    bool BBFrustomCulling(const BoundingBox3D &bb, const Matrix4f &view, const Matrix4f &modelMat)
    {
        auto vertices = bb.GetVertices();
        for (auto vert : vertices)
        {
            Vector4f posOS = Vector4f(vert.x(), vert.y(), vert.z(), 1);
            Vector4f posWS = modelMat * posOS;
            Vector4f posVS = view * posWS;
            if (posVS.z() > 0)
                return true;
        }
        return false;
    }
    struct VertShaderResult
    {
        vector<VertInputStandard> vins;
        vector<VertOutputStandard> vertOuts;
    };

    /// @brief 面毎に頂点シェーダーを実行し、結果を返す
    /// @param faceIndex
    /// @param model
    /// @param in
    /// @param vert
    /// @return
    VertShaderResult ExecVertShaderEachFace(
        const int faceIndex,
        Model &model,
        const VertInputStandard &in,
        const VertOutputStandard (*vert)(const VertInputStandard &in))
    {
        VertInputStandard vin = in;
        VertShaderResult result;

        const vector<int> &face = model.facesID[faceIndex];
        vin.material = &model.GetMaterialFromFaceIndex(faceIndex);

        // 面を構成する各頂点IDについてFor
        vector<VertOutputStandard> outs;
        vector<VertInputStandard> vins;

        result.vins.reserve(face.size());
        result.vertOuts.reserve(face.size());
        for (size_t vertIndex = 0; vertIndex < face.size(); vertIndex++)
        {
            vin.position = model.GetPositionFromFaceIndexAndVertIndex(faceIndex, vertIndex);
            vin.normal = model.GetNormalFromFaceIndexAndVertIndex(faceIndex, vertIndex);
            vin.uv = model.GetUVFromFaceIndexAndVertIndex(faceIndex, vertIndex);
            ApplyVAT(vin, vertIndex, in.environment.time, model.VATPos, model.VATNormal);

            VertOutputStandard out = vert(vin); // 頂点シェーダーでクリップ座標系に変換
            result.vertOuts.push_back(out);     // 描画待ち配列に追加
            result.vins.push_back(vin);
        }
        return result;
    }

    vector<vector<VertOutputStandard>> ExecTessellationPass(
        VertShaderResult &vertResult,
        const vector<int> &face,
        const VertInputStandard &vin,
        const VertOutputStandard (*vert)(const VertInputStandard &in))
    {
        int tessCount = 0;
        // バンプマップがあるときのみ分割
        if (vin.material && vin.material->bumpMap)
            tessCount = Tessellation::CalcTessLevel(
                vertResult.vertOuts[0].positionSS.head<2>(),
                vertResult.vertOuts[1].positionSS.head<2>(),
                vertResult.vertOuts[2].positionSS.head<2>(), 200, vin.environment.MaxTesselleateCount);
        auto tessVIns = Tessellation::TessellateSpecificArea(vertResult.vins, tessCount);
        vector<vector<VertOutputStandard>> tessVOuts;
        tessVOuts.reserve(tessVIns.size());
        for (size_t tessIndex = 0; tessIndex < tessVIns.size(); tessIndex++)
        {
            vector<VertOutputStandard> tmpouts;
            tmpouts.reserve(face.size());
            for (size_t vertIndex = 0; vertIndex < face.size(); vertIndex++)
                tmpouts.push_back(vert(tessVIns[tessIndex][vertIndex]));
            tessVOuts.push_back(tmpouts);
        }
        return tessVOuts;
    }

    bool ShouldCullFace(const vector<VertOutputStandard> &vertices, const RenderingEnvironmentParameters &env)
    {
        Vector3f normal = GeometryMath::ComputeFaceNormal(
            vertices[0].positionVS.head<3>(),
            vertices[1].positionVS.head<3>(),
            vertices[2].positionVS.head<3>());

        Vector3f center = (vertices[0].positionVS + vertices[1].positionVS + vertices[2].positionVS).head<3>() / 3;
        float orientation = normal.dot(center);

        // バックフェースカリングのチェック
        bool backfaceCull = env.backFaceCullingDirection * orientation >= 0 || !env.backfaceCulling;

        return backfaceCull && isAnyVertInFrustum(vertices);
    }
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
                if (face.size() < 3) // 三角面未満なら描画できない
                    continue;
                auto vertResult = ExecVertShaderEachFace(faceIndex, model, vin, vert);

                auto tessVOuts = ExecTessellationPass(vertResult, face, vin, vert);

                // 分割された各面について
                for (size_t tessedFaceIndex = 0; tessedFaceIndex < tessVOuts.size(); tessedFaceIndex++)
                {
                    if (ShouldCullFace(tessVOuts[tessedFaceIndex], in.environment))
                    {
                        // 面を一つ描画
                        SimpleDefferedFillPolygon(RenderingPipeline::VertOuts2PixcelIns(tessVOuts[tessedFaceIndex]), gb, *pixcel);
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
                        if (out.alpha < 0.1f)
                            continue;

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

            int N = tessellateCount;
            //  N回分割開始
            for (int i = 0; i < N; i++)
            {
                vector<vector<PixcelInputStandard>> next;
                // もともと入っていた各頂点に対して分割
                for (unsigned long int i = 0; i < logs.back().size(); i++)
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
                for (unsigned long int i = 0; i < logs.back().size(); i++)
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
                if (face.size() < 3) // 三角面未満なら描画できない
                    continue;
                auto vertResult = ExecVertShaderEachFace(faceIndex, model, vin, vert);

                auto tessVOuts = ExecTessellationPass(vertResult, face, vin, vert);

                // 分割された各面について
                for (size_t tessedFaceIndex = 0; tessedFaceIndex < tessVOuts.size(); tessedFaceIndex++)
                {
                    auto tessOut = tessVOuts[tessedFaceIndex];
                    if (ShouldCullFace(tessOut, in.environment))
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
