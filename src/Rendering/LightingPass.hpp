#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <omp.h>
#include "../header/EigenHeader.hpp"
#include "../header/ShaderHeader.hpp"
#include "../Models/Model.hpp"
#include "../TransformMat.hpp"
#include "GBuffers.hpp"
#include "RenderTarget.hpp"
using namespace std;

namespace RenderingPass
{

    void ExecLightingPass(GBuffers &gbuffers,
                          const Vector3f (*postProcessPass)(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y),
                          RenderingEnvironmentParameters &environment);
}