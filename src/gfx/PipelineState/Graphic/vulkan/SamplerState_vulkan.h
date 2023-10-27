#pragma once

#include "gfx/PipelineState/Graphic/SamplerState.h"

namespace vg::gfx::vulkan
{
    class SamplerState : public base::SamplerState
    {
        using super = base::SamplerState;

    public:
        static VkSamplerCreateInfo getVulkanSamplerState(Sampler _sampler);

    private:
        static VkFilter getVulkanFilter(Filter _filter);
        static VkSamplerMipmapMode getVulkanMipmapMode(Filter _filter);
        static VkSamplerAddressMode getVulkanAddressMode(Address _address);
    };
}