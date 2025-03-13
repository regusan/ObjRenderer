#include "PostProcessShader.hpp"

namespace PostProcessShader
{

    Vector3f SimpleBloom(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y)
    {
        int kernelSize = 12;
        int kernelScale = 8;
        Vector3f sum = Vector3f(0, 0, 0);
        for (int lx = -kernelSize / 2; lx < kernelSize / 2; lx++)
        {
            for (int ly = -kernelSize / 2; ly < kernelSize / 2; ly++)
            {
                Vector3f dif = gbuffers.diffuse.SampleColor(x + lx * kernelScale, y + ly * kernelScale);
                sum += Vector3f(clamp<float>(dif.x(), 0, 1), clamp<float>(dif.y(), 0, 1), clamp<float>(dif.z(), 0, 1));
            }
        }

        sum /= kernelSize * kernelSize;
        return sum + gbuffers.beauty.SampleColor(x, y);
    }

    void BloomWithDownSampling(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, float bloomThreshold, float intencity)
    {
        // ダウンサンプリングしたバッファを用意

        vector<RenderTarget::DownSampleData> cinemaDSD = {
            RenderTarget::DownSampleData(gbuffers.screenSize / 1, 3),
            RenderTarget::DownSampleData(gbuffers.screenSize / 2, 5),
            RenderTarget::DownSampleData(gbuffers.screenSize / 4, 9),
            RenderTarget::DownSampleData(gbuffers.screenSize / 8, 17),
            RenderTarget::DownSampleData(gbuffers.screenSize / 16, 33),
            RenderTarget::DownSampleData(gbuffers.screenSize / 32, 33),
            RenderTarget::DownSampleData(gbuffers.screenSize / 64, 33),
        };
        vector<RenderTarget::DownSampleData> midDSD = {
            RenderTarget::DownSampleData(gbuffers.screenSize / 4, 3),
            RenderTarget::DownSampleData(gbuffers.screenSize / 8, 5),
            RenderTarget::DownSampleData(gbuffers.screenSize / 16, 7),
            RenderTarget::DownSampleData(gbuffers.screenSize / 32, 15),
        };
        vector<RenderTarget::DownSampleData> dsd = (environment.quality == RenderingQuality::Cinema) ? cinemaDSD : midDSD;

        RenderTarget bloomSource(gbuffers.screenSize.x(), gbuffers.screenSize.y());
#pragma omp parallel for
        for (int y = 0; y < bloomSource.getScreenSize().y(); y++)
            for (int x = 0; x < bloomSource.getScreenSize().x(); x++)
            {
                Vector3f beauty = gbuffers.beauty.SampleColor(x, y);
                Vector3f overflow = Vector3f(fmax(0, beauty.x() / bloomThreshold - 1), fmax(0, beauty.y() / bloomThreshold - 1), fmax(0, beauty.z() / bloomThreshold - 1));
                bloomSource.PaintPixel(x, y, overflow);
            }

        vector<RenderTarget> downSampledBuffers = bloomSource.GausiannBlurWithDownSample(dsd);
        for (int y = 0; y < bloomSource.getScreenSize().y(); y++)
            for (int x = 0; x < bloomSource.getScreenSize().x(); x++)
            {
                // ダウンサンプリングしたバッファを合成
                Vector3f sum = Vector3f(0, 0, 0);
                for (size_t i = 0; i < downSampledBuffers.size(); i++)
                    sum += downSampledBuffers[i].SampleColor(x, y);
                gbuffers.beauty.PaintPixel(x, y, sum / dsd.size() * intencity + gbuffers.beauty.SampleColor(x, y));
            }
    }
    void BloomWithMultipleConv(GBuffers &gbuffers, RenderingEnvironmentParameters &environment)
    {
        // ダウンサンプリングしたバッファを用意
        int num = 5;
        int kernelSize = 32;
        RenderTarget current(gbuffers.screenSize.x(), gbuffers.screenSize.y());
#pragma omp parallel for
        for (int y = 0; y < current.getScreenSize().y(); y++)
            for (int x = 0; x < current.getScreenSize().x(); x++)
            {
                Vector3f beauty = gbuffers.beauty.SampleColor(x, y);
                Vector3f overflow = Vector3f(fmax(0, beauty.x() - 1), fmax(0, beauty.y() - 1), fmax(0, beauty.z() - 1));
                current.PaintPixel(x, y, overflow);
            }

        for (int i = 1; i < num; i++)
        {
            current = current.GausiannBlur(kernelSize, true);
        }
#pragma omp parallel for
        for (int y = 0; y < current.getScreenSize().y(); y++)
            for (int x = 0; x < current.getScreenSize().x(); x++)
            {
                gbuffers.beauty.PaintPixel(x, y, current.SampleColor(x, y) + gbuffers.beauty.SampleColor(x, y));
            }
    }
    void ScreenSpaceAmbientOcculusionCryTek(GBuffers &gbuffers, RenderingEnvironmentParameters &environment)
    {
        int maxSampleNum = (environment.quality == RenderingQuality::Cinema) ? 200 : 10;
        int NoiseCount = maxSampleNum * 10;
        int skipSize = 1; // 何ピクセルおきに計算するか
        float sphereRadius = 10;
        // ノイズを事前計算
        vector<Vector3f> noises;
        uint precomputeSeed = 1;
        for (int i = 0; i < NoiseCount; i++)
        {
            noises.push_back(GeometryMath::MakeRandomPointInSphereByUVSeed(Vector3f(0, 0, 0), sphereRadius, precomputeSeed));
            precomputeSeed = GeometryMath::xorshift(precomputeSeed);
        }

#pragma omp parallel for
        for (int y = 0; y < gbuffers.depth.getScreenSize().y(); y += skipSize)
        {
            uint seed = y + environment.time + 1;
            for (int x = 0; x < gbuffers.depth.getScreenSize().x(); x += skipSize)
            {
                // 深度が無限遠だったら処理しない
                if (gbuffers.depth.SampleColor(x, y).x() == numeric_limits<float>::max())
                {
                    continue;
                }
                int visibleCount = 0; // 可視サンプルのカウンタ
                Vector3f positionVS = gbuffers.positionVS.SampleColor(x, y);
                Vector3f normalVS = gbuffers.normalVS.SampleColor(x, y);
                for (int i = 0; i < maxSampleNum; i++)
                {
                    Vector3f noise = noises[seed % NoiseCount];
                    Vector3f randomVS = positionVS;
                    randomVS += (noise.dot(normalVS) > 0) ? noise : -noise; // 半球状に修正
                    seed = GeometryMath::xorshift(seed);
                    const Vector3f randomSS = (randomVS / randomVS.z() + Vector3f(1, 1, 1)) * 0.5;
                    float factDepth = gbuffers.positionVS.SampleColor01(randomSS.x(), randomSS.y()).z();
                    visibleCount += factDepth > randomVS.z(); // サンプル点が実際の深度より手前だったらカウント
                }
                float ratio = fmin(1, static_cast<float>(visibleCount) / maxSampleNum); // 可視サンプル数から比率を計算
                gbuffers.AO.PaintPixel(x, y, Vector3f(ratio, ratio, ratio));
            }
        }
        if (environment.quality == RenderingQuality::Cinema)
            gbuffers.AO = gbuffers.AO.GausiannBlur(3);
    }
    void SSAOPlusSSGI(GBuffers &gbuffers, RenderingEnvironmentParameters &environment)
    {
        int maxSampleNum = (environment.quality == RenderingQuality::Cinema) ? 500 : 10;
        int NoiseCount = maxSampleNum * 10;
        int skipSize = 1;                               // 何ピクセルおきに計算するか
        constexpr float sphereRadius = 1;               // ランダムにサンプルする球の半径1
        constexpr float invisibleDepthThreshold = 1.0f; // この深度以上の遮蔽は可視判定にする→アウトラインの発生抑制

        //  ノイズを事前計算
        vector<Vector3f> noises;
        uint precomputeSeed = 1;
        for (int i = 0; i < NoiseCount; i++)
        {
            noises.push_back(GeometryMath::MakeRandomPointInSphereByUVSeed(Vector3f(0, 0, 0), sphereRadius, precomputeSeed));
            precomputeSeed = GeometryMath::xorshift(precomputeSeed);
        }

#pragma omp parallel for
        for (int y = 0; y < gbuffers.depth.getScreenSize().y(); y += skipSize)
        {
            uint seed = y + environment.time + 1;
            for (int x = 0; x < gbuffers.depth.getScreenSize().x(); x += skipSize)
            {
                // 深度が無限遠だったら処理しない
                if (gbuffers.depth.SampleColor(x, y).x() == numeric_limits<float>::max())
                {
                    continue;
                }
                int visibleCount = 0; // 可視サンプルのカウンタ
                Vector3f positionVS = gbuffers.positionVS.SampleColor(x, y);
                Vector3f normalVS = gbuffers.normalVS.SampleColor(x, y);
                Vector3f bouncedColor = Vector3f(0, 0, 0);
                // float sumOfStrength = 0.0f;
                for (int i = 0; i < maxSampleNum; i++)
                {
                    Vector3f noise = noises[seed % NoiseCount];
                    Vector3f randomVS = positionVS;
                    randomVS += (noise.dot(normalVS) > 0) ? noise : -noise; // 半球状に修正
                    seed = GeometryMath::xorshift(seed);
                    const Vector3f randomSS = (randomVS / randomVS.z() + Vector3f(1, 1, 1)) * 0.5;
                    float factDepth = gbuffers.positionVS.SampleColor01(randomSS.x(), randomSS.y()).z();
                    // レイが画面外に行ったら強制終了
                    if (randomSS.x() < 0 || randomSS.x() >= 1 || randomSS.y() < 0 || randomSS.y() >= 1)
                    {
                        visibleCount++;
                        continue;
                    }
                    // サンプル点が実際の深度より手前、あるいは遮蔽地点との距離が離れすぎているなら可視判定
                    if (factDepth - randomVS.z() > 0 || randomVS.z() - factDepth > invisibleDepthThreshold)
                    {
                        visibleCount++;
                        Vector3f factPosVS = Vector3f(randomVS.x(), randomVS.y(), factDepth);
                        Vector3f factNormalVS = gbuffers.normalVS.SampleColor01(randomSS.x(), randomSS.y());
                        // Vector3f reflectVS = MathPhysics::Reflect(lightDirVS, factNormalVS).normalized();
                        // Vector3f random2sampleVS = (factPosVS - positionVS);
                        float affectRateByDistance = 1 - (factPosVS - positionVS).norm() / sphereRadius;    // 近いほど大きくなる影響度合い
                        float affectRateByAngle = clamp<float>((1 - factNormalVS.dot(normalVS)) / 2, 0, 1); // 正面になるほど大きくなる影響度合い
                        float strength = affectRateByAngle * affectRateByDistance;
                        bouncedColor = bouncedColor + gbuffers.beauty.SampleColor01(randomSS.x(), randomSS.y()) * strength;
                    }
                }
                float ratio = fmin(1, static_cast<float>(visibleCount) / maxSampleNum); // 可視サンプル数から比率を計算
                bouncedColor = bouncedColor / visibleCount;
                gbuffers.irradiance.PaintPixel(x, y, Vector3f(fmax(bouncedColor.x(), 0), fmax(bouncedColor.y(), 0), fmax(bouncedColor.z(), 0)));
                gbuffers.AO.PaintPixel(x, y, Vector3f(ratio, ratio, ratio));
            }
        }
        if (environment.quality == RenderingQuality::Cinema)
        {
            gbuffers.AO = gbuffers.AO.GausiannBlur(5);
            gbuffers.irradiance = gbuffers.irradiance.GausiannBlur(15);
        }
    }
    void ScreenSpaceReflection(GBuffers &gbuffers, RenderingEnvironmentParameters &environment)
    {
        // 光線を飛ばす回数
        const int maxRayNum = (environment.quality == RenderingQuality::Cinema) ? 200 : 30;
        // レイの最大距離
        const float maxRayLength = (environment.quality == RenderingQuality::Cinema) ? 10 : 6;
        const float rayLength = maxRayLength / maxRayNum;
        // 自分自身に反射するのを防ぐための最小距離
        const float minimumLength = rayLength * 0;

        float maxThickness = 1.f / maxRayNum;

#pragma omp parallel for
        for (int y = 0; y < gbuffers.reflection.getScreenSize().y(); y++)
        {
            uint seed = y + environment.time + 1;
            for (int x = 0; x < gbuffers.reflection.getScreenSize().x(); x++)
            {
                // 深度が無限遠だったら処理しない
                if (gbuffers.depth.SampleColor(x, y).x() == numeric_limits<float>::max())
                {
                    continue;
                }

                const Vector3f positionVS = gbuffers.positionVS.SampleColor(x, y);
                Vector3f normalVS = (gbuffers.normalVS.SampleColor(x, y)); //* (1 - roughness) + noises[seed % NoiseCount] * roughness).normalized();
                Vector3f reflect = MathPhysics::Reflect(positionVS, normalVS).normalized();
                // TODO:二分探索にして高速化する
                for (int i = 1; i <= maxRayNum; i++)
                {
                    float noise = seed % 1000 / 1000.0f;
                    const float currentRayLength = rayLength * (i + noise) + minimumLength;
                    seed = GeometryMath::xorshift(seed);
                    const Vector3f offsetVS = reflect * currentRayLength;
                    const Vector3f rayPosVS = positionVS + offsetVS;
                    const Vector3f rayPosSS = (rayPosVS / rayPosVS.z() + Vector3f(1, 1, 1)) * 0.5;
                    // レイが画面外に行ったら強制終了
                    if (rayPosSS.x() < 0 || rayPosSS.x() > 1 || rayPosSS.y() < 0 || rayPosSS.y() > 1)
                        break;

                    // GBufferから取得した実際の深度
                    const float actualDepth = gbuffers.positionVS.SampleColor01(rayPosSS.x(), rayPosSS.y()).z();
                    const float actualBackDepth = gbuffers.backPositionVS.SampleColor01(rayPosSS.x(), rayPosSS.y()).z();
                    // レイが背面深度と表面深度の間にあったら
                    if (rayPosVS.z() > actualDepth && ((rayPosVS.z() < actualBackDepth) || rayPosVS.z() - actualDepth < maxThickness))
                    {
                        Vector2f screenCoord(2.0f * static_cast<float>(x) / gbuffers.reflection.getScreenSize().x() - 1.0f,
                                             2.0f * static_cast<float>(y) / gbuffers.reflection.getScreenSize().y() - 1.0f);
                        float farness = clamp<float>(screenCoord.norm(), 0.0f, 1.0f);
                        float strength = 1 - farness * farness; // 1-x^2で急激なフォールオフ

                        gbuffers.reflection.PaintPixel(x, y, gbuffers.preBeauty.SampleColor01(rayPosSS.x(), rayPosSS.y()));
                        gbuffers.reflectionLevel.PaintPixel(x, y, Vector3f(strength, strength, strength));
                        break;
                    }
                }
            }
        }
        if (environment.quality == RenderingQuality::Cinema)
            gbuffers.reflection = gbuffers.reflection.GausiannBlur(3);
    }
    void ScreenSpaceShadow(GBuffers &gbuffers, RenderingEnvironmentParameters &environment)
    {
        // 光線を飛ばす回数
        const int maxRayNum = (environment.quality == RenderingQuality::Cinema) ? 1000 : 30;
        // レイの最大距離
        const float maxRayLength = (environment.quality == RenderingQuality::Cinema) ? 10 : 10;
        const float rayLength = maxRayLength / maxRayNum;
        // 自分自身に反射するのを防ぐための最小距離
        const float minimumLength = maxRayLength * 0.01;

        // float maxThickness = 30.0f / maxRayNum;
        Vector3f lightWS = environment.directionalLights.at(0).direction;
        Vector3f normalVS = (Transform::ResetPosition(ResetScale(environment.viewMat)) * Vector4f(lightWS.x(), lightWS.y(), lightWS.z(), 1)).head<3>().normalized();

#pragma omp parallel for
        for (int y = 0; y < gbuffers.screenSize.y(); y++)
        {
            uint seed = y + environment.time + 1;
            for (int x = 0; x < gbuffers.screenSize.x(); x++)
            {
                // 深度が無限遠だったら処理しない
                if (gbuffers.depth.SampleColor(x, y).x() == numeric_limits<float>::max())
                {
                    continue;
                }

                const Vector3f positionVS = gbuffers.positionVS.SampleColor(x, y);
                // normalVS = Vector3f(1, 1, -1).normalized();
                //  TODO:二分探索にして高速化する
                for (int i = 1; i <= maxRayNum; i++)
                {

                    float noise = seed % 1000 / 1000.0f;
                    const float currentRayLength = rayLength * (i + noise) + minimumLength;
                    seed = GeometryMath::xorshift(seed);
                    const Vector3f offsetVS = normalVS * currentRayLength;
                    const Vector3f rayPosVS = positionVS + offsetVS;
                    const Vector3f rayPosSS = (rayPosVS / rayPosVS.z() + Vector3f(1, 1, 1)) * 0.5;
                    // レイが画面外に行ったら強制終了
                    if (rayPosSS.x() < 0 || rayPosSS.x() > 1 || rayPosSS.y() < 0 || rayPosSS.y() > 1)
                        break;

                    // GBufferから取得した実際の深度
                    const float actualDepth = gbuffers.positionVS.SampleColor01(rayPosSS.x(), rayPosSS.y()).z();
                    const float actualBackDepth = gbuffers.backPositionVS.SampleColor01(rayPosSS.x(), rayPosSS.y()).z();
                    // レイが背面深度と表面深度の間にあるか、実際の深度とレイの深度が近いなら遮蔽判定
                    if (rayPosVS.z() > actualDepth && (rayPosVS.z() < actualBackDepth || rayPosVS.z() - actualDepth < .01))
                    {
                        gbuffers.SSShadow.PaintPixel(x, y, Vector3f(0, 0, 0));
                        break;
                    }
                }
            }
        }
        gbuffers.SSShadow = gbuffers.SSShadow.GausiannBlur(5, false, false);
        if (environment.quality == RenderingQuality::Cinema)
            gbuffers.SSShadow = gbuffers.SSShadow.GausiannBlur(11);
    }

    void AutoExposure(GBuffers &gbuffers, RenderingEnvironmentParameters &environment)
    {
        constexpr float middleGrey = 0.50f;

        constexpr float sampleRate = 0.01;
        int steps = static_cast<int>(1.0f / sampleRate);
        constexpr int skip = 2;
        int count = 0;
        constexpr float gannma = 0.45;
        Vector3f sum = Vector3f::Zero();
        float sum2 = 0;

        // #pragma omp parallel for
        for (int y = 0; y < gbuffers.screenSize.y(); y += skip)
        {
            for (int x = 0; x < gbuffers.screenSize.x(); x += skip)
            {
                sum2 += gbuffers.beauty.SampleColor(x, y).norm();
            }
        }
        sum2 /= gbuffers.screenSize.y() / skip * gbuffers.screenSize.x() / skip;
        float avarageBrightness = sum2;
        // avarageBrightness = gbuffers.beauty.SampleColor01BiLinear(0.5, 0.5).norm();
        // cout << avarageBrightness << endl;

#pragma omp parallel for
        for (int y = 0; y < gbuffers.screenSize.y(); y++)
        {
            for (int x = 0; x < gbuffers.screenSize.x(); x++)
            {
                const Vector3f sampled = gbuffers.beauty.SampleColor(x, y);
                float brightness = sampled.norm();

                float exporedBrightness = brightness * (middleGrey / avarageBrightness);
                float rate = exporedBrightness / (exporedBrightness + 1); // tonemap
                rate = powf(rate, gannma);
                gbuffers.beauty.PaintPixel(x, y, sampled.normalized() * rate);
            }
        }
    }
}