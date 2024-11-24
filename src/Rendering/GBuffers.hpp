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
    RenderTarget forward;
    RenderTarget beauty;
    RenderTarget diffuse;

    RenderTarget depth;

    RenderTarget positionWS;
    RenderTarget positionVS;

    RenderTarget normalWS;
    RenderTarget normalVS;

    RenderTarget uv;

    GBuffers(const int &width, const int &height);
    ~GBuffers();

    // ファイルに出力する関数
    void writeAsPPM(const string &filepath,
                    const float positionModValue = 1,
                    const float normalMulValue = 255);
};
