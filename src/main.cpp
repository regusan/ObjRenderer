#include "Model.hpp"
#include <iostream>
#include "shaderlab/StandardShader.hpp"
#include "TransformMat.hpp"
#include "header/EigenHeader.hpp"
#include "header/RenderingHeader.hpp"
using namespace std;
using namespace Transform;
int main(int argc, char const *argv[])
{
    cout << "起動" << endl;

    VertInputStandard in;
    in.viewMat = Transform::MakeMatOffset(Vector3f(0, 3, 10)) * Transform::MakeRotMat(Vector3f(180, 90, 0));

    GBuffers gb = GBuffers(1000, 1000);

    Model model = Model();
    // model.loadObj("models/room.obj");
    if (argc >= 2)
        model.loadObj(argv[1]);
    else
        perror("INVALID ARGS\n");
    // RenderingPipeline::Forward::SimpleForwardDrawModelWireframe(model, in, rt, VertStandard, PixcelStandard);
    RenderingPipeline::Deffered::DefferedDrawModel(model, in, gb, VertStandard, PixcelStandard);
    gb.writeAsPPM("outputs/out", .5);
    cout << "output" << endl;
    return 0;
}