
#include <iostream>
#include <chrono>
#include <filesystem>
#include "shaderlab/StandardShader.hpp"
#include "shaderlab/LightingPassShader.hpp"
#include "shaderlab/PostProcessShader.hpp"
#include "Rendering/PostProcessPass.hpp"
#include "TransformMat.hpp"
#include "header/EigenHeader.hpp"
#include "header/RenderingHeader.hpp"
#include "GameObject/TurnTableCamera.hpp"
#include "GameObject/FPSCamera.hpp"
#include "GUI/X11Display.hpp"
#include "STL/EventDispatcher.hpp"
#include "STL/ConfigParser.hpp"
#include "Models/Material.hpp"
#include "Models/Model.hpp"

#include "lib/stb/stb_image.h"
#include "lib/stb/stb_image_write.h"

using namespace std;
using namespace Transform;

void UpdateInput(const XEvent &event) {}
EventDispatcher<XEvent> inputDispatcher;

ConfigParser config = ConfigParser("config.ini");
RenderingEnvironmentParameters environment = RenderingEnvironmentParameters();
VertInputStandard in = VertInputStandard(environment);
Model primaryModel = Model();

int main(int argc, char const *argv[])
{
    // 初期化処理
    cout << "起動" << endl;
    cout << config << endl;

    in.environment.loadFromConfig(config);

    if (argc >= 2)
        primaryModel.LoadModelFromFile(argv[1]);
    else
    {
        fprintf(stderr, "Invalid args:takes 1 arguments but %c were given.\nUsage:%s <ObjFiepath> \n", argc, argv[0]);
        exit(1);
    }
    X11Display display(in.environment.screenSize.x(), in.environment.screenSize.y());
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
    GBuffers gb = GBuffers(environment.screenSize.x(), environment.screenSize.y());
    while (true)
    {
        // 時間の計測
        auto start = std::chrono::high_resolution_clock::now();

        // GBufferの初期化
        gb.Clear();
        // 視点の更新
        if (in.environment.cameraMoveMode == CameraMoveMode::FPS)
        {
            fpsCamera.SetRotation(Vector3f(0, display.GetMousePos().x(), display.GetMousePos().y()));
            in.viewMat = fpsCamera.getMat();
        }
        else if (in.environment.cameraMoveMode == CameraMoveMode::TurnTable)
        {
            turnTableCamera.SetRotation(Vector3f(0, display.GetMousePos().x(), display.GetMousePos().y()));
            in.viewMat = turnTableCamera.getMat();
        }
        in.environment.viewMat = in.viewMat;
        in.environment.setCurrentTIme();
        // 各レンダリングパスを実行
        RenderingPipeline::Deffered::ExecGeometryPass(primaryModel, in, gb, VertStandard, PixcelStandard);
        // Low未満ではそもそもパスを実行しない
        if (in.environment.quality > RenderingQuality::Low)
        {
            PostProcessShader::ScreenSpaceAmbientOcculusionCryTek(gb, in.environment);
            PostProcessShader::ScreenSpaceShadow(gb, in.environment);
        }
        RenderingPass::ExecLightingPass(gb, DefferedLightingPassShader, in.environment);
        if (in.environment.quality > RenderingQuality::Low)
        {
            PostProcessShader::ScreenSpaceReflection(gb, in.environment);
        }
        RenderingPass::ExecLightingPass(gb, FinalLightingPassShader, in.environment);
        if (in.environment.quality > RenderingQuality::Low)
        {
            PostProcessShader::BloomWithDownSampling(gb, in.environment);
        }
        //   GBufferからデバイスコンテキストにコピー
        RenderTarget &rt = gb.getRTFromString(config.GetAsString("Buffer2Display"));
        display.show(rt);

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
                        config = ConfigParser("config.ini");
                        in.environment.loadFromConfig(config);
                        // gb = GBuffers(environment.screenSize.x(), environment.screenSize.y());
                        // display.Resize(environment.screenSize);
                        break;
                    case XK_space: // スナップショットを記録
                    {              // 括弧で囲わないとローカル変数定義できない
                        cout << "キャプチャ開始" << endl;
                        Vector2i preResolution = in.environment.screenSize;
                        RenderingQuality preQuality = in.environment.quality;

                        in.environment.screenSize = Vector2i(2048, 2048);
                        in.environment.quality = RenderingQuality::Cinema;

                        GBuffers higb = GBuffers(environment.screenSize.x(), environment.screenSize.y());
                        RenderingPipeline::Deffered::ExecGeometryPass(primaryModel, in, higb, VertStandard, PixcelStandard);
                        PostProcessShader::ScreenSpaceAmbientOcculusionCryTek(higb, environment);
                        PostProcessShader::ScreenSpaceShadow(higb, environment);
                        RenderingPass::ExecLightingPass(higb, DefferedLightingPassShader, environment);
                        PostProcessShader::ScreenSpaceReflection(higb, environment);
                        RenderingPass::ExecLightingPass(higb, FinalLightingPassShader, environment);
                        PostProcessShader::BloomWithDownSampling(higb, environment);
                        cout << "レンダリング終了" << endl;

                        auto now = std::chrono::system_clock::now();
                        auto now_time_t = std::chrono::system_clock::to_time_t(now);

                        // 時刻を人間可読形式で表示
                        std::tm local_tm = *std::localtime(&now_time_t);
                        ostringstream outputPath;
                        outputPath << "outputs/out_" << std::put_time(&local_tm, "%Y-%m-%d-%H-%M-%S");
                        higb.writeAsPNG(outputPath.str(), 1); // 書き出し
                        cout << outputPath.str() << "にキャプチャ完了" << endl;

                        in.environment.screenSize = preResolution;
                        in.environment.quality = preQuality;
                    }
                    break;
                    case XK_Escape: // 終了処理
                        display.~X11Display();
                        exit(0);
                    }
                }
                auto end = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                cout << "FPS::" << 1000 / elapsed.count() << endl;
            }
        }
    }
    return 0;
}