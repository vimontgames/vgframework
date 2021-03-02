#pragma once

#include "graphics/driver/PipelineState/DepthStencilState.h"

namespace vg::graphics::driver::vulkan
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