#pragma once

#include "../header/EigenHeader.hpp"
#include "../header/ShaderHeader.hpp"
#include "RenderTarget.hpp"
#include <limits>
using namespace std;

class GBuffers
{
private:
    /* data */
public:
    Vector2i screenSize;
    RenderTarget beauty;
    RenderTarget diffuse;

    RenderTarget depth;

    RenderTarget positionWS;

    RenderTarget normalWS;
    RenderTarget normalCS;

    GBuffers(const int &width, const int &height);
    ~GBuffers();

    // ファイルに出力する関数
    void writeAsPPM(const string &filepath);
};
