#pragma once

#include "gfx/FrameGraph/UserPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class RenderPass : public gfx::UserPass
    {
    public:
        RenderPass(const core::string & _name);
        ~RenderPass();
    };
}