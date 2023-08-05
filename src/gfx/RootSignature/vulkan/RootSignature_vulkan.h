#pragma once

#include "gfx/RootSignature/RootSignature.h"

namespace vg::gfx::vulkan
{
    class RootSignature : public base::RootSignature
    {
        using super = base::RootSignature;

    public:
        RootSignature(const RootSignatureDesc & _desc);
        ~RootSignature();

        VkPipelineLayout getVulkanPipelineLayout() const { return m_vkPipelineLayout; }
        const core::vector<VkDescriptorSetLayout> & getVulkanDescriptorSetLayouts() const { return m_vkDescriptorSetLayouts; }

        static VkShaderStageFlags getVulkanShaderStageFlags(ShaderStageFlags _shaderStageFlags);

    private:
        VkPipelineLayout                    m_vkPipelineLayout;
        core::vector<VkDescriptorSetLayout> m_vkDescriptorSetLayouts;
    };
}