
#include <iostream>
#include <chrono>
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

#include "GUI/X11Display.hpp"
#include "STL/EventDispatcher.hpp"
#include "Models/Material.hpp"
#include "Models/Model.hpp"
#include "Rendering/PostProcessPass.hpp"

#include "lib/stb/stb_image.h"
#include "lib/stb/stb_image_write.h"

#include <nlohmann/json.hpp>

using namespace std;
using namespace Transform;

/// @brief Jsonコンフィグファイルを開く処理をまとめた関数
/// @return
nlohmann::json JsonOpener(string filePath);
void UpdateInput(const XEvent &event) {}

EventDispatcher<XEvent> inputDispatcher;

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
        sceneFileName = argv[1]; // 引数のモデルをロード
        scene.loadFromJson(JsonOpener(sceneFileName));
        scene.ExecBeginPlay();
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
    TurnTableCamera turnTableCamera;
    FPSCamera fpsCamera;
    turnTableCamera.SetPosition(Vector3f(0, 3, 0));
    fpsCamera.SetPosition(Vector3f(0, 0, 10));
    inputDispatcher.addListener([&turnTableCamera](const XEvent &event)
                                {
                                    turnTableCamera.OnUpdateInput(event); // メンバ関数を呼び出す
                                });
    inputDispatcher.addListener([&fpsCamera](const XEvent &event)
                                {
                                    fpsCamera.OnUpdateInput(event); // メンバ関数を呼び出す
                                });

    // 描画先初期化
    X11Display display(environment.screenSize.x(), environment.screenSize.y());
    GBuffers gb = GBuffers(environment.screenSize.x(), environment.screenSize.y());

    while (true)
    {
        //  時間の計測
        auto start = std::chrono::high_resolution_clock::now();

        // GBufferのクリア
        gb.Clear();
        // 視点の更新
        if (environment.cameraMoveMode == CameraMoveMode::FPS)
        {
            fpsCamera.SetRotation(Vector3f(0, display.GetMousePos().x(), display.GetMousePos().y()));
            in.viewMat = fpsCamera.getMat();
        }
        else if (environment.cameraMoveMode == CameraMoveMode::TurnTable)
        {
            turnTableCamera.SetRotation(Vector3f(0, display.GetMousePos().x(), display.GetMousePos().y()));
            in.viewMat = turnTableCamera.getMat();
        }
        environment.viewMat = in.viewMat;
        environment.setCurrentTIme();
        environment.lights = scene.GetObjectsOfClass<LightBaseActor>();
        fpsCamera.speed = turnTableCamera.speed = environment.cameraSpeed;

        // 各レンダリングパスを実行
        if (environment.quality >= RenderingQuality::Low)
        {
            auto meshes = scene.GetObjectsOfClass<MeshActor>();
            for (auto &mesh : meshes)
            {
                in.modelMat = mesh->getMat();
                RenderingPipeline::Deffered::ExecGeometryPass(mesh->meshModel, in, gb, VertStandard, PixcelStandard);
            }

            // RenderingPipeline::Lighting::ExecLightGeometryPass(primaryModel, in, gb, VertStandard, PixcelStandard);

            // Low未満ではそもそもパスを実行しない
            if (environment.quality > RenderingQuality::Low)
            {
                PostProcessShader::ScreenSpaceShadow(gb, environment);
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
                PostProcessShader::BloomWithDownSampling(gb, environment, 5.0f);
            }
            PostProcessShader::AutoExposure(gb, environment);
        }
        else
        {
            auto meshes = scene.GetObjectsOfClass<MeshActor>();
            for (auto &mesh : meshes)
            {
                in.modelMat = mesh->getMat();
                RenderingPipeline::Forward::ExecWireFramePass(mesh->meshModel, in, gb, VertStandard);
            }
        }

        //   GBufferからデバイスコンテキストにコピーし、表示
        RenderTarget &rt = gb.getRTFromString(environment.buffer2Display);
        display.show(rt);

        // 経過時間の取得とTickの実行
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); // ms
        scene.ExecTick(static_cast<float>(elapsed.count()) / 1000.0f);

        // イベント処理
        XEvent event;
        if (XPending(display.GetDisplay()) > 0)
        {
            XNextEvent(display.GetDisplay(), &event);
            inputDispatcher.dispatch(event);
            switch (event.type)
            {
            case KeyPress:
                // キーが押された場合
                {
                    KeySym key = XLookupKeysym(&event.xkey, 0);
                    // キーごとに処理を分ける
                    switch (key)
                    {

                    case XK_Return: // コンフィグのリロード
                        environment.loadFromJson(JsonOpener(configFileName));
                        scene = Scene();
                        scene.loadFromJson(JsonOpener(sceneFileName));
                        scene.ExecBeginPlay();
                        // gb = GBuffers(environment.screenSize.x(), environment.screenSize.y());
                        // display.Resize(environment.screenSize);
                        break;
                    case XK_space: // スナップショットを記録
                    {              // 括弧で囲わないとローカル変数定義できない
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
                            in.modelMat = mesh->getMat();
                            RenderingPipeline::Deffered::ExecGeometryPass(mesh->meshModel, in, prehigb, VertStandard, PixcelStandard);
                        }
                        PostProcessShader::ScreenSpaceShadow(prehigb, environment);
                        PostProcessShader::ScreenSpaceReflection(prehigb, environment);
                        RenderingPass::ExecTileBasedLightingPass(prehigb, LighingShader::IBLShader, environment);

                        GBuffers higb = GBuffers(environment.screenSize.x(), environment.screenSize.y());
                        higb.preBeauty = prehigb.beauty;
                        for (auto &mesh : meshes)
                        {
                            in.modelMat = mesh->getMat();
                            RenderingPipeline::Deffered::ExecGeometryPass(mesh->meshModel, in, higb, VertStandard, PixcelStandard);
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
                    break;
                    case XK_Escape: // 終了処理
                        display.~X11Display();
                        exit(0);
                    // レンダリングクオリティの変更
                    case XK_1:
                        environment.quality = RenderingQuality::Wire;
                        break;
                    case XK_2:
                        environment.quality = RenderingQuality::Low;
                        break;
                    case XK_3:
                        environment.quality = RenderingQuality::Mid;
                        break;
                    case XK_4:
                        environment.quality = RenderingQuality::Cinema;
                        break;
                    }
                }

                // FPSとアセット状況を表示
                cout << "FPS::" << 1000 / elapsed.count() << endl;
                cout << "テクスチャアセット管理状況:" << AssetSubSystem::getInstance().textureManager << endl;
            }
        }
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