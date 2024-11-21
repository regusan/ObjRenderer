#include "Model.hpp"
#include "RenderTarget.hpp"
#include <iostream>
#include "shader.hpp"
#include "Math3D.hpp"
#include "RenderingLibrary.hpp"

#include "header/EigenHeader.hpp"
using namespace std;
int main(int argc, char const *argv[])
{
    cout << "起動" << endl;

    VertInputStandard in;
    in.viewMat = MakeMatOffset(Vector3f(0, 3, 10)) * MakeRotMat(Vector3f(180 + 30, 0, 0));

    RenderTarget rt = RenderTarget(500, 500, Vector3f(100, 100, 100));

    Model model = Model();
    // model.loadObj("models/room.obj");
    if (argc >= 2)
        model.loadObj(argv[1]);
    else
        perror("INVALID ARGS\n");
    DrawModel(model, in, rt, vert, pixcel);

    rt.writeAsPPM("outputs/out.ppm");
    cout << "output" << endl;
    return 0;
}