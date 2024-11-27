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

    while (true)
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        GBuffers gb = GBuffers(size.x(), size.y());
        in.viewMat = Transform::MakeMatOffset(Vector3f(0.0, 5, 10.0)) * Transform::MakeRotMat(Vector3f(180, elapsed.count() * 0.01, 0));
        RenderingPipeline::Deffered::DefferedDrawModel(model, in, gb, VertStandard, PixcelStandard);
        RenderTarget rt = gb.normalVS;
        display.show(rt);
        gb.writeAsPPM("outputs/out", .5);
    }

    display.waitUntilWindowBreak();
    cout << "output" << endl;
    return 0;
}