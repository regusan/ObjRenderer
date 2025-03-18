#include "LightingPass.hpp"
namespace RenderingPass
{
    void ExecScanPass(GBuffers &gbuffers,
                      const Vector3f (*lightingPass)(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y),
                      RenderingEnvironmentParameters &environment)
    {
//  行ごとに並列処理
#pragma omp parallel for
        for (int y = 0; y < gbuffers.screenSize.y(); y++)
        {
            for (int x = 0; x < gbuffers.screenSize.x(); x++)
            {
                gbuffers.beauty.PaintPixel(x, y, lightingPass(gbuffers, environment, x, y));
            }
        }
    }

    void ExecTileBasedLightingPass(GBuffers &gbuffers,
                                   const Vector3f (*lightingPass)(GBuffers &gbuffers, RenderingEnvironmentParameters &environment,
                                                                  vector<shared_ptr<LightBaseActor>> lights, int x, int y),
                                   RenderingEnvironmentParameters &environment)
    {
        constexpr int tileCountXY = 8;
        const Vector2i tilePixelSize = gbuffers.screenSize / tileCountXY;

        struct lightBoundData
        {
            float radiusSS;
            Vector2i centorSS;
            shared_ptr<LightBaseActor> light;
        };
        // ライトのスクリーンスペースでの位置と半径を事前計算
        vector<lightBoundData> lightBounds;
        lightBounds.reserve(environment.lights.size());
        for (auto light : environment.lights)
        {
            lightBoundData lightBound;
            Vector4f posOS = light->getWorldMat().col(3);
            posOS.w() = 1;
            Vector4f posVS = environment.viewMat * posOS;

            lightBound.centorSS = Vector2i((posVS.x() / posVS.z() + 1) * 0.5 * environment.screenSize.x(),
                                           (posVS.y() / posVS.z() + 1) * 0.5 * environment.screenSize.y());
            lightBound.radiusSS = fmax(0, light->GetBoundingSphereRadius() / posVS.z() * environment.screenSize.x());
            lightBound.light = light;
            lightBounds.push_back(lightBound);
        }

        //  各タイルごとに並列処理
        // #pragma omp parallel for
        for (int tileY = 0; tileY < tileCountXY; tileY++)
        {
            for (int tileX = 0; tileX < tileCountXY; tileX++)
            {
                // タイルごとにライトとの衝突判定とる
                // このタイルに影響するライトのリスト
                vector<shared_ptr<LightBaseActor>> affectLights;
                affectLights.reserve(lightBounds.size());
                BoundingBox tileBB(tileX * tilePixelSize.x(),
                                   (tileX + 1) * tilePixelSize.x(),
                                   tileY * tilePixelSize.y(),
                                   (tileY + 1) * tilePixelSize.y());
                for (auto lightBound : lightBounds)
                {
                    // タイルBBとライトBSを囲うBBとの衝突判定を取る

                    BoundingBox lightBB(lightBound.centorSS.cast<float>(), lightBound.radiusSS);
                    if (tileBB & lightBB)
                    {
                        affectLights.push_back(lightBound.light);
                    }
                }

                for (int y = tileY * tilePixelSize.y(); y < (tileY + 1) * tilePixelSize.y(); y++)
                {
                    for (int x = tileX * tilePixelSize.x(); x < (tileX + 1) * tilePixelSize.x(); x++)
                    {
                        gbuffers.beauty.PaintPixel(x, y, lightingPass(gbuffers, environment, affectLights, x, y));
                    }
                }
            }
        }
    }
    void ExecLightingPass(GBuffers &gbuffers,
                          const Vector3f (*lightingPass)(GBuffers &gbuffers, RenderingEnvironmentParameters &environment,
                                                         vector<shared_ptr<LightBaseActor>> lights, int x, int y),
                          RenderingEnvironmentParameters &environment)
    { //  行ごとに並列処理
#pragma omp parallel for
        for (int y = 0; y < gbuffers.screenSize.y(); y++)
        {
            for (int x = 0; x < gbuffers.screenSize.x(); x++)
            {
                gbuffers.beauty.PaintPixel(x, y, lightingPass(gbuffers, environment, environment.lights, x, y));
            }
        }
    }
}