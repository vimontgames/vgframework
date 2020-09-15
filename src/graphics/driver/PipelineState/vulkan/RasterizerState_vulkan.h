#pragma once

#include "graphics/driver/PipelineState/RasterizerState.h"

namespace vg::graphics::driver::vulkan
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