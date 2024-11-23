#include "Model.hpp"
#include "Rendering/RenderTarget.hpp"
#include <iostream>
#include "shaderlab/StandardShader.hpp"
#include "Rendering/RenderingLibrary.hpp"
#include "TransformMat.hpp"
#include "header/EigenHeader.hpp"
using namespace std;
using namespace Transform;
int main(int argc, char const *argv[])
{
    cout << "起動" << endl;

    VertInputStandard in;
    in.viewMat = Transform::MakeMatOffset(Vector3f(0, 3, 10)) * Transform::MakeRotMat(Vector3f(180 + 30, 0, 0));

    RenderTarget rt = RenderTarget(1000, 1000, Vector3f(100, 100, 100));

    Model model = Model();
    // model.loadObj("models/room.obj");
    if (argc >= 2)
        model.loadObj(argv[1]);
    else
        perror("INVALID ARGS\n");
    RenderingPipeline::SimpleDrawModelWireframe(model, in, rt, VertStandard, PixcelStandard);

    rt.writeAsPPM("outputs/out.ppm");
    cout << "output" << endl;
    return 0;
}