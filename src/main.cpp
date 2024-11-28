#include "Model.hpp"
#include <iostream>
#include <chrono>
#include "shaderlab/StandardShader.hpp"
#include "TransformMat.hpp"
#include "header/EigenHeader.hpp"
#include "header/RenderingHeader.hpp"
#include "GameObject/TurnTableCamera.hpp"
#include "GUI/X11Display.hpp"
#include "STL/EventDispatcher.hpp"
#include "STL/ConfigParser.hpp"

using namespace std;
using namespace Transform;
void UpdateInput(const XEvent &event) {}
EventDispatcher<XEvent> inputDispatcher;

VertInputStandard initFromConfig(ConfigParser config, VertInputStandard original)
{
    original.nearClip = config.GetAsNumeric("NearClip");
    original.farClip = config.GetAsNumeric("NearClip");
    original.backfaceCulling = config.GetAsBool("BackfaceCulling");
    return original;
}
ConfigParser config = ConfigParser("config.ini");

int main(int argc, char const *argv[])
{
    cout << "起動" << endl;
    Vector2i size = Vector2i(800, 800);

    VertInputStandard in;
    in = initFromConfig(config, in);

    Model model = Model();
    // model.loadObj("models/room.obj");
    if (argc >= 2)
        model.loadObj(argv[1]);
    else
        perror("INVALID ARGS\n");

    X11Display display(size.x(), size.y());
    TurnTableCamera camera;
    camera.SetPosition(Vector3f(0, 4, 0));
    inputDispatcher.addListener([&camera](const XEvent &event)
                                {
                                    camera.OnUpdateInput(event); // メンバ関数を呼び出す
                                });

    cout << config << endl;
    bool running = true;
    while (true)
    {
        // 時間の計測
        auto start = std::chrono::high_resolution_clock::now();

        // GBufferの定義
        GBuffers gb = GBuffers(size.x(), size.y());

        // 視点の更新
        camera.SetRotation(Vector3f(display.GetMousePos().y(), display.GetMousePos().x(), 0));
        in.viewMat = camera.getMat();

        // GBufferに格納
        RenderingPipeline::Deffered::DefferedDrawModel(model, in, gb, VertStandard, PixcelStandard);

        // GBufferからデバイスコンテキストにコピー
        RenderTarget rt = gb.getRTFromString(config.GetAsString("Buffer2Display")) % 1;
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
                        in = initFromConfig(config, in);

                        break;
                    }
                }

                if (!running) // 破棄
                {
                    display.~X11Display();
                    gb.writeAsPPM("outputs/out", .5); // 書き出し
                    break;
                }
                auto end = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                cout << "FPS::" << 1000 / elapsed.count() << endl;
            }
        }
    }
    return 0;
}