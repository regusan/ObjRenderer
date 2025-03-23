
#include <iostream>
#include <filesystem>
#include <sstream>

#include "shaderlab/StandardShader.hpp"
#include "shaderlab/LightingPassShader.hpp"
#include "shaderlab/PostProcessShader.hpp"

#include "header/MathHeader.hpp"
#include "header/EigenHeader.hpp"
#include "header/RenderingHeader.hpp"

#include "GameObject/Camera/TurnTableCamera.hpp"
#include "GameObject/Camera/FPSCamera.hpp"
#include "GameObject/Scene.hpp"
#include "GameObject/Mesh.hpp"

#include "Engine/Input/InputSubSystem.hpp"
#include "Engine/Data/PrimitiveMeshData.hpp"
#include "Engine/SubSystem/DebugDrawSubSystem.hpp"

#include "GUI/X11Display.hpp"
#include "GUI/X11toREvent.hpp"

#include "Models/Material.hpp"
#include "Models/Model.hpp"
#include "Rendering/PostProcessPass.hpp"

#include "lib/stb/stb_image.h"
#include "lib/stb/stb_image_write.h"

#include <nlohmann/json.hpp>

using namespace std;
using namespace Transform;
using namespace REngine;
using namespace REngine::Input;

/// @brief Jsonコンフィグファイルを開く処理をまとめた関数
/// @return
nlohmann::json JsonOpener(string filePath);

/// @brief シーンのリロード処理をまとめた関数
/// @param scenefileName
void ReloadScene(string scenefileName);

/// @brief シーンのヒエラルキーを書き出し
/// @param scene
/// @param file
void UpdateHierarchyLog(Scene &scene, ofstream file);

/// @brief リソース書き出し
/// @param file
void UpdateResourceLog(ofstream file);

/// @brief 高解像度スナップショットを撮る
void TakeHiResSnapShot();

RenderingEnvironmentParameters environment = RenderingEnvironmentParameters();
VertInputStandard in = VertInputStandard(environment);
Scene scene = Scene();
string sceneFileName = "";

const string configFileName = "config.json";
int main(int argc, char const *argv[])
{
    // 初期化処理
    cout << "起動" << endl;

    if (argc >= 2)
    {
        ReloadScene(argv[1]);
    }
    else
    {
        fprintf(stderr, "Invalid args:takes 1 arguments but %c were given.\nUsage:%s <ObjFiepath> \n", argc, argv[0]);
        exit(1);
    }

    // コンフィグの読み取り
    environment.loadFromJson(JsonOpener(configFileName));
    cout << environment << endl;

    // カメラ初期設定
    shared_ptr<Camera> primaryCamera;

    // 描画先初期化
    X11Display display(environment.screenSize.x() * environment.upscaleRate, environment.screenSize.y() * environment.upscaleRate);
    GBuffers gb = GBuffers(environment.screenSize.x(), environment.screenSize.y());

    while (true)
    {
        scene.ExecBeginPlay();
        // Tick実行
        float deltasecond = scene.ExecTick();
        scene.ExecDestroyObject();

        // 初めに取得したカメラでレンダリング設定
        if (primaryCamera)
        {
            environment.viewMat = in.viewMat = primaryCamera->getWorldMat().inverse();
            primaryCamera->speed = environment.cameraSpeed;
        }
        // カメラがなかったら取得を試みる
        else if (!scene.GetObjectsOfClass<Camera>().empty())
        {
            primaryCamera = scene.GetObjectsOfClass<Camera>()[0].lock();
        }

        // ライトの収集
        auto lightswp = scene.GetObjectsOfClass<LightBaseActor>();
        environment.setCurrentTIme();
        environment.lights.clear();
        environment.lights.reserve(lightswp.size());
        for (auto &wp : lightswp)
        {
            if (auto sp = wp.lock())
                environment.lights.push_back(sp);
        }

        // 各レンダリングパスを実行
        // GBufferのクリア
        gb.Clear();
        if (environment.quality >= RenderingQuality::Low)
        {
            auto meshes = scene.GetObjectsOfClass<MeshActor>();
            // cout << scene << endl;
            for (auto &mesh : meshes)
            {
                if (auto sp = mesh.lock())
                {
                    in.modelMat = sp->getWorldMat();
                    RenderingPipeline::Deffered::ExecGeometryPass(*sp->meshModel, in, gb, VertStandard, PixcelStandard);
                }
            }

            // RenderingPipeline::Lighting::ExecLightGeometryPass(primaryModel, in, gb, VertStandard, PixcelStandard);

            // Low未満ではそもそもパスを実行しない
            if (environment.quality > RenderingQuality::Low)
            {
                // PostProcessShader::ScreenSpaceShadow(gb, environment);
                PostProcessShader::ScreenSpaceReflection(gb, environment);
            }
            RenderingPass::ExecLightingPass(gb, LighingShader::IBLShader, environment);
            if (environment.quality > RenderingQuality::Low)
            {
                PostProcessShader::SSAOPlusSSGI(gb, environment);
            }
            RenderingPass::ExecScanPass(gb, LighingShader::BackGroundLighingShader, environment);
            if (environment.quality > RenderingQuality::Low)
            {
                PostProcessShader::BloomWithDownSampling(gb, environment, 10.0f);
            }
            PostProcessShader::AutoExposure(gb, environment);
        }
        else
        {
            auto meshes = scene.GetObjectsOfClass<MeshActor>();
            for (auto mesh : meshes)
            {
                if (auto sp = mesh.lock())
                {
                    in.modelMat = sp->getWorldMat();
                    RenderingPipeline::Forward::ExecWireFramePass(*sp->meshModel, in, gb, VertStandard);
                }
            }
        }
        if (environment.drawDebugShape)
        { // デバッグ用メッシュをワイヤフレームで描画
            for (auto &model : DebugDrawSubSystem::getInstance().wireFrameDrawCall)
            {
                in.modelMat = model->getWorldMat();
                RenderingPipeline::Forward::ExecWireFramePass(*model->meshModel, in, gb, VertStandard);
            }
            DebugDrawSubSystem::getInstance().wireFrameDrawCall.clear();
        }

        //   GBufferからデバイスコンテキストにコピーし、表示
        RenderTarget &rt = gb.getRTFromString(environment.buffer2Display);
        display.show(rt.UpSample(environment.screenSize.array() * environment.upscaleRate));

        // ログに書き出し
        UpdateHierarchyLog(scene, ofstream("hierarchy.log"));
        UpdateResourceLog(ofstream("resource.log"));

        // イベント更新
        InputSubSystem::getInstance().axisState.UpdateAxisState(scene.timeManager.GetDeltatime(), Vector3f(display.GetMousePos().x(), display.GetMousePos().y(), 0));
        InputSubSystem::getInstance().UpdateKeyStatus(scene.timeManager.GetDeltatime(), X11Event2REvent(display.GetDisplay()));
        // cout << InputSubSystem::getInstance() << endl;
        if (InputSubSystem::getInstance().GetKeyStatus(KeyID::Enter).isPressed)
        {
            environment.loadFromJson(JsonOpener(configFileName));
            ReloadScene(sceneFileName);
        }

        if (InputSubSystem::getInstance().GetKeyStatus(KeyID::Space).isPressed)
        {
            TakeHiResSnapShot();
        }

        if (InputSubSystem::getInstance().GetKeyStatus(KeyID::Escape).isPressed)
        {
            display.~X11Display();
            exit(0);
        }

        if (InputSubSystem::getInstance().GetKeyStatus(KeyID::Num1).isPressed)
            environment.quality = RenderingQuality::Wire;

        if (InputSubSystem::getInstance().GetKeyStatus(KeyID::Num2).isPressed)
            environment.quality = RenderingQuality::Low;

        if (InputSubSystem::getInstance().GetKeyStatus(KeyID::Num3).isPressed)
            environment.quality = RenderingQuality::Mid;

        if (InputSubSystem::getInstance().GetKeyStatus(KeyID::Num4).isPressed)
            environment.quality = RenderingQuality::Cinema;
        if (InputSubSystem::getInstance().GetKeyStatus(KeyID::Num5).isPressed)
            environment.drawDebugShape = !environment.drawDebugShape;
    }
    return 0;
}

nlohmann::json JsonOpener(const std::string filePath)
{

    // ファイルを開く
    std::ifstream file(filePath);

    if (!file)
    {
        std::cerr << "Failed to open file " << filePath << std::endl;
        exit(1);
    }

    // ファイルの内容を文字列に読み込む
    std::stringstream buffer;
    buffer << file.rdbuf();

    // 文字列に変換
    std::string content = buffer.str();

    // 読み込んだ内容を表示
    // std::cout << "File content: \n"<< content << std::endl;

    // ファイルを閉じる
    file.close();
    auto jobj = nlohmann::json::parse(content);
    return jobj;
}

void ReloadScene(string scenefileName)
{
    sceneFileName = scenefileName; // 引数のモデルをロード
    scene.loadScene(filesystem::path(sceneFileName));
}

void UpdateHierarchyLog(Scene &scene, ofstream file)
{
    if (file.is_open())
    {
        file << scene.hieralcyToString().str();
        file.close();
    }
}

void UpdateResourceLog(ofstream file)
{
    if (file.is_open())
    {
        file << AssetSubSystem::getInstance() << endl;
        file.close();
    }
}

void TakeHiResSnapShot()
{
    cout << "キャプチャ開始" << endl;
    Vector2i preResolution = environment.screenSize;
    RenderingQuality preQuality = environment.quality;

    environment.screenSize = Vector2i(2048, 2048);
    environment.quality = RenderingQuality::Cinema;

    auto meshes = scene.GetObjectsOfClass<MeshActor>();

    // 反射キャプチャのための事前レンダリング
    GBuffers prehigb = GBuffers(environment.screenSize.x(), environment.screenSize.y());

    for (auto &mesh : meshes)
    {
        if (auto sp = mesh.lock())
        {
            in.modelMat = sp->getWorldMat();
            RenderingPipeline::Deffered::ExecGeometryPass(*sp->meshModel, in, prehigb, VertStandard, PixcelStandard);
        }
    }
    PostProcessShader::ScreenSpaceShadow(prehigb, environment);
    PostProcessShader::ScreenSpaceReflection(prehigb, environment);
    RenderingPass::ExecTileBasedLightingPass(prehigb, LighingShader::IBLShader, environment);

    GBuffers higb = GBuffers(environment.screenSize.x(), environment.screenSize.y());
    higb.preBeauty = prehigb.beauty;
    for (auto &mesh : meshes)
    {
        if (auto sp = mesh.lock())
        {
            in.modelMat = sp->getWorldMat();
            RenderingPipeline::Deffered::ExecGeometryPass(*sp->meshModel, in, higb, VertStandard, PixcelStandard);
        }
    }
    PostProcessShader::ScreenSpaceShadow(higb, environment);
    PostProcessShader::ScreenSpaceReflection(higb, environment);
    RenderingPass::ExecTileBasedLightingPass(higb, LighingShader::IBLShader, environment);
    PostProcessShader::SSAOPlusSSGI(higb, environment);
    PostProcessShader::BloomWithDownSampling(higb, environment, 5);
    PostProcessShader::AutoExposure(higb, environment);
    RenderingPass::ExecScanPass(higb, LighingShader::BackGroundLighingShader, environment);

    cout << "レンダリング終了" << endl;

    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    // 時刻を人間可読形式で表示
    std::tm local_tm = *std::localtime(&now_time_t);
    ostringstream outputPath;
    outputPath << "outputs/out_" << std::put_time(&local_tm, "%Y-%m-%d-%H-%M-%S");
    higb.writeAsPNG(outputPath.str(), 1); // 書き出し
    cout << outputPath.str() << "にキャプチャ完了" << endl;

    environment.screenSize = preResolution;
    environment.quality = preQuality;
}