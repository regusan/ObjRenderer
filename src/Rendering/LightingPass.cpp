#include "LightingPass.hpp"
namespace RenderingPass
{
    void ExecLightingPass(GBuffers &gbuffers,
                          const Vector3f (*lightingPass)(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y),
                          RenderingEnvironmentParameters &environment)
    {
//  行ごとに並列処理
#pragma omp parallel for
        for (size_t y = 0; y < gbuffers.screenSize.y(); y++)
        {
            for (size_t x = 0; x < gbuffers.screenSize.x(); x++)
            {
                gbuffers.beauty.PaintPixel(x, y, lightingPass(gbuffers, environment, x, y));
            }
        }
    }
}