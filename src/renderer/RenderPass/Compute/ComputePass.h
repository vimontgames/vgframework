#pragma once

#include "renderer/RenderPass/RenderPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ComputePass : public RenderPass
    {
    public:
        ComputePass(const core::string & _passName);
        ~ComputePass();
    };
}