#pragma once

#include "renderer/RenderPass/RenderPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class RenderObjectsPass : public RenderPass
    {
    public:
        RenderObjectsPass(const core::string & _name);
        ~RenderObjectsPass();
    };
}