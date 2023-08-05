#pragma once

#include "gfx/PipelineState/RasterizerState.h"

namespace vg::gfx::vulkan
{
    class RasterizerState : public base::RasterizerState
    {
        using super = base::RasterizerState;

    public:
        VkPipelineRasterizationStateCreateInfo  getVulkanRasterizerState() const;

        static VkPolygonMode getVulkanPolygonMode(FillMode _fillMode);
        static VkCullModeFlagBits getVulkanCullMode(CullMode _cullMode);

    private:
    };
}