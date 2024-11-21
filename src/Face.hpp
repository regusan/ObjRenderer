#pragma once
#include "Model.hpp"
#include "RenderTarget.hpp"
#include "RenderingLibrary.hpp"

#include <iostream>

#include "header/EigenHeader.hpp"
#include "header/ShaderHeader.hpp"
using namespace std;

class Face
{
private:
    /* data */
public:
    Face(/* args */);
    ~Face();
    vector<VertOutputStandard> verts;
    PixcelInputStandard Lerp(Vector3f posDS);
};

Face::Face(/* args */)
{
}

Face::~Face()
{
}
