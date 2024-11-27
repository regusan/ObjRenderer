#include "Model.hpp"
#include <iostream>
#include <chrono>
#include "shaderlab/StandardShader.hpp"
#include "TransformMat.hpp"
#include "header/EigenHeader.hpp"
#include "header/RenderingHeader.hpp"

#include "GUI/X11Display.hpp"
using namespace std;
using namespace Transform;
int main(int argc, char const *argv[])
{
    Vector2i size = Vector2i(500, 500);
    cout << "起動" << endl;

    VertInputStandard in;
    in.viewMat = Transform::MakeMatOffset(Vector3f(0, 3, 10)) * Transform::MakeRotMat(Vector3f(180, 50, 0));

    Model model = Model();
    // model.loadObj("models/room.obj");
    if (argc >= 2)
        model.loadObj(argv[1]);
    else
        perror("INVALID ARGS\n");

    X11Display display(size.x(), size.y());

    auto start = std::chrono::high_resolution_clock::now();
    bool running = true;
    while (true)
    {
        // 時間の計測
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        // GBufferの定義
        GBuffers gb = GBuffers(size.x(), size.y());

        // 視点の更新
        in.viewMat = Transform::MakeMatOffset(Vector3f(0.0, 5, 10.0)) * Transform::MakeRotMat(Vector3f(display.GetMousePos().y(), display.GetMousePos().x(), 0));

        // GBufferに格納
        RenderingPipeline::Deffered::DefferedDrawModel(model, in, gb, VertStandard, PixcelStandard);

        // GBufferからデバイスコンテキストにコピー
        display.show(gb.forward);
        XEvent event;
        if (XPending(display.GetDisplay()) > 0)
        {
            XNextEvent(display.GetDisplay(), &event);
            switch (event.type)
            {
            case KeyPress:
                // キーが押された場合
                std::cout << "Key pressed: " << XKeysymToString(XKeycodeToKeysym(display.GetDisplay(), event.xkey.keycode, 0)) << std::endl;
                break;
            case ClientMessage:
                // WM_DELETE_WINDOWイベントが発生した場合（ウィンドウが閉じられた）
                if (static_cast<Atom>(event.xclient.data.l[0]) == display.WM_DELETE_WINDOW)
                {
                    std::cout << "Window close detected!" << std::endl;
                    running = false; // ウィンドウを閉じる
                }
                break;

            default:
                break;
            }
        }

        if (!running) // 破棄
        {
            gb.writeAsPPM("outputs/out", .5); // 書き出し
            display.~X11Display();
            break;
        }
    }

    return 0;
}