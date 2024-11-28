#pragma once

#include "../header/EigenHeader.hpp"
#include "../header/ShaderHeader.hpp"
#include "RenderTarget.hpp"
#include <limits>
#include <map>
using namespace std;

class GBuffers
{
private:
    map<string, RenderTarget *> str2rt;

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
                    const float normalMulValue = 1);
    RenderTarget &getRTFromString(string rtname);
};
