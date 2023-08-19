#pragma once

#include "renderer/RenderPass/RenderPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class Render2DPass : public RenderPass
    {
    public:
        Render2DPass(const core::string & _passName);
        ~Render2DPass();
    };
}