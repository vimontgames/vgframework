#include "UpdatePass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    UpdatePass::UpdatePass(const core::string & _name) :
        RenderPass(_name)
    {
        setUserPassType(RenderPassType::Copy);
    }

    //--------------------------------------------------------------------------------------
    UpdatePass::~UpdatePass()
    {

    }
}