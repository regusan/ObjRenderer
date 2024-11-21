#pragma once
#include "ShaderSettings.hpp"
#include "Model.hpp"
#include "RenderTarget.hpp"
#include "RenderingLibrary.hpp"

#include <iostream>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>
using Eigen::Matrix4f;
using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Vector4f;
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
