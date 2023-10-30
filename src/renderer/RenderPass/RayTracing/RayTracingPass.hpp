#include "RayTracingPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    RayTracingPass::RayTracingPass(const core::string & _name) :
        RenderPass(_name)
    {
        setUserPassType(RenderPassType::RayTrace);
    }

    //--------------------------------------------------------------------------------------
    RayTracingPass::~RayTracingPass()
    {

    }
}