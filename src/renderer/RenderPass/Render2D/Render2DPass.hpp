#include "Render2DPass.h"

#include "Background/BackgroundPass.hpp"
#include "FinalBlit/FinalBlitPass.hpp"
#include "HDROutput/HDROutputPass.hpp"
#include "ResolveDeferredMSAA/ResolveDeferredMSAAPass.hpp"
#include "LinearizeDepth/LinearizeDepthPass.hpp"
#include "Preview/Texture/TexturePreviewPass.hpp"

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