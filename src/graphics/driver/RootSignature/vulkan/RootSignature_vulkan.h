#pragma once

#include "graphics/driver/RootSignature/RootSignature.h"

namespace vg::graphics::driver::vulkan
{
    class RootSignature : public base::RootSignature
    {
        using super = base::RootSignature;

    public:
        RootSignature(const RootSignatureDesc & _desc);
        ~RootSignature();

        static VkShaderStageFlags getVulkanShaderStageFlags(ShaderStageFlags _shaderStageFlags);

    private:
        VkPipelineLayout                    m_vkPipelineLayout;
        core::vector<VkDescriptorSetLayout> m_vkDescriptorSetLayouts;
    };
}