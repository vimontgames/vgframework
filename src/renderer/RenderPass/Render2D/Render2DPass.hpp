#include "Render2DPass.h"

#include "Render2D/Background/BackgroundPass.hpp"
#include "Render2D/FinalBlit/FinalBlitPass.hpp"
#include "Render2D/HDROutput/HDROutputPass.hpp"

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