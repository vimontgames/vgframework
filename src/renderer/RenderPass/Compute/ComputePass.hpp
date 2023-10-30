#include "ComputePass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ComputePass::ComputePass(const core::string & _name) :
        RenderPass(_name)
    {
        setUserPassType(RenderPassType::Compute);
    }

    //--------------------------------------------------------------------------------------
    ComputePass::~ComputePass()
    {

    }
}