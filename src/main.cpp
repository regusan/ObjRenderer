#include "Model.hpp"
#include <iostream>
#include "shaderlab/StandardShader.hpp"
#include "TransformMat.hpp"
#include "header/EigenHeader.hpp"
#include "header/RenderingHeader.hpp"

#include "GUI/X11Display.hpp"
using namespace std;
using namespace Transform;
int main(int argc, char const *argv[])
{
    Vector2i size = Vector2i(1000, 1000);
    cout << "起動" << endl;

    VertInputStandard in;
    in.viewMat = Transform::MakeMatOffset(Vector3f(0, 3, 10)) * Transform::MakeRotMat(Vector3f(180, 50, 0));

    GBuffers gb = GBuffers(size.x(), size.y());

    Model model = Model();
    // model.loadObj("models/room.obj");
    if (argc >= 2)
        model.loadObj(argv[1]);
    else
        perror("INVALID ARGS\n");
    RenderingPipeline::Deffered::DefferedDrawModel(model, in, gb, VertStandard, PixcelStandard);
    gb.writeAsPPM("outputs/out", .5);
    X11Display display(size.x(), size.y());
    while (true)
    {
        display.show(gb.forward);
    }

    display.waitUntilWindowBreak();
    cout << "output" << endl;
    return 0;
}