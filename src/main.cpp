
#include <iostream>
#include <chrono>
#include "shaderlab/StandardShader.hpp"
#include "shaderlab/LightingPassShader.hpp"
#include "shaderlab/PostProcessShader.hpp"
#include "Rendering/PostProcessPass.hpp"
#include "TransformMat.hpp"
#include "header/EigenHeader.hpp"
#include "header/RenderingHeader.hpp"
#include "GameObject/TurnTableCamera.hpp"
#include "GUI/X11Display.hpp"
#include "STL/EventDispatcher.hpp"
#include "STL/ConfigParser.hpp"
#include "Models/Material.hpp"
#include "Models/Model.hpp"

using namespace std;
using namespace Transform;
void UpdateInput(const XEvent &event) {}
EventDispatcher<XEvent> inputDispatcher;

ConfigParser config = ConfigParser("config.ini");

int main(int argc, char const *argv[])
{
    cout << "起動" << endl;

    cout << config << endl;
    VertInputStandard in;
    in.environment.loadFromConfig(config);

    Model model = Model();
    // model.loadObj("models/room.obj");
    if (argc >= 2)
        model.LoadModelFromFile(argv[1]);
    else
    {
        fprintf(stderr, "Invalid args:takes 1 arguments but %c were given.\nUsage:%s <ObjFiepath> \n", argc, argv[0]);
        exit(1);
    }
    X11Display display(in.environment.screenSize.x(), in.environment.screenSize.y());
    TurnTableCamera camera;
    camera.SetPosition(Vector3f(0, 3, 0));
    inputDispatcher.addListener([&camera](const XEvent &event)
                                {
                                    camera.OnUpdateInput(event); // メンバ関数を呼び出す
                                });
    while (true)
    {
        // 時間の計測
        auto start = std::chrono::high_resolution_clock::now();

        // GBufferの定義
        GBuffers gb = GBuffers(in.environment.screenSize.x(), in.environment.screenSize.y());

        // 視点の更新
        camera.SetRotation(Vector3f(display.GetMousePos().y(), display.GetMousePos().x(), 0));
        in.viewMat = camera.getMat();
        in.environment.viewMat = in.viewMat;
        in.environment.setCurrentTIme();

        // GBufferに格納
        RenderingPipeline::Deffered::ExecGeometryPass(model, in, gb, VertStandard, PixcelStandard);
        PostProcessShader::ScreenSpaceAmbientOcculusionCryTek(gb, in.environment);
        PostProcessShader::ScreenSpaceShadow(gb, in.environment);
        RenderingPass::ExecLightingPass(gb, DefferedLightingPassShader, in.environment);
        PostProcessShader::ScreenSpaceReflection(gb, in.environment);
        PostProcessShader::BloomWithDownSampling(gb);

        // GBufferからデバイスコンテキストにコピー
        RenderTarget rt = gb.getRTFromString(config.GetAsString("Buffer2Display"));
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

                        break;
                    case XK_Escape:
                        display.~X11Display();
                        gb.beauty.DownSample(Vector2i(100, 100)).UpSample(Vector2i(500, 500)).writeAsPPM("outputs2.ppm");
                        gb.writeAsPPM("outputs", .5); // 書き出し

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