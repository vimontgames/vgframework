#pragma once

#include "gfx/PipelineState/Graphic/DepthStencilState.h"

namespace vg::gfx::vulkan
{
    class DepthStencilState : public base::DepthStencilState
    {
        using super = base::DepthStencilState;

    public:
        VkPipelineDepthStencilStateCreateInfo getVulkanDepthStencilState() const;

        static VkCompareOp getVulkanCompareFunc(ComparisonFunc _cmpFunc);
        static VkStencilOp getVulkanStencilOp(StencilOp _stencilOp);

    private:
    };
}