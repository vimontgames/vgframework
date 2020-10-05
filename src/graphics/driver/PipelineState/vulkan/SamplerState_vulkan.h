#pragma once

#include "graphics/driver/PipelineState/SamplerState.h"

namespace vg::graphics::driver::vulkan
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