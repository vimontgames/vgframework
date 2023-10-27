#pragma once

#include "renderer/RenderPass/RenderPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class RayTracingPass : public RenderPass
    {
    public:
        RayTracingPass(const core::string & _passName);
        ~RayTracingPass();

    private:

    };
}