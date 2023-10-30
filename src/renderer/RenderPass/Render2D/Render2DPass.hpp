#include "Render2DPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    Render2DPass::Render2DPass(const core::string & _name) :
        RenderPass(_name)
    {
        setUserPassType(RenderPassType::Graphic);
    }

    //--------------------------------------------------------------------------------------
    Render2DPass::~Render2DPass()
    {

    }
}