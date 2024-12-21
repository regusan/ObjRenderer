#include "CommonRenderingLibrary.hpp"
namespace RenderingPipeline
{
    vector<PixcelInputStandard> VertOuts2PixcelIns(vector<VertOutputStandard> outs)
    {
        vector<PixcelInputStandard> pis;
        for (VertOutputStandard v : outs)
        {
            pis.push_back(PixcelInputStandard(v));
        }
        return pis;
    }
}