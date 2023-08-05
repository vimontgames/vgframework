#pragma once

#include "gfx/PipelineState/BlendState.h"

namespace vg::gfx::vulkan
{
    class BlendState : public base::BlendState
    {
        using super = base::BlendState;

    public:
        VkPipelineColorBlendStateCreateInfo getVulkanBlendState(VkPipelineColorBlendAttachmentState * _vkRenderTargetBlend, core::uint _renderTargetCount) const;

    private:
        static VkBlendFactor getVulkanBlendFactor(BlendFactor _blendFactor);
        static VkBlendOp getVulkanBlendOp(BlendOp _blendOp);
    };
}