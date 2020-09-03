namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    RootSignature::RootSignature(const RootSignatureDesc & _desc) :
        super::RootSignature(_desc)
    {
        core::vector<VkPushConstantRange> vkRootConstants;

        const auto & rootConstants = _desc.getRootConstants();
        for (uint i = 0; i < rootConstants.size(); ++i)
        {
            const RootSignatureDesc::PushConstantParams & rootConstant = rootConstants[i];

            VkPushConstantRange vkRootConstant;

            vkRootConstant.offset = rootConstant.m_start;
            vkRootConstant.size = rootConstant.m_count * sizeof(u32);
            vkRootConstant.stageFlags = getVulkanShaderStageFlags(rootConstant.m_stages);

            vkRootConstants.push_back(vkRootConstant);
        }

        for (uint i = 0; i < _desc.getTables().size(); ++i)
        {
            // TODO: add VkDescriptorSetLayoutCreateInfo 
        }

        VkPipelineLayoutCreateInfo vkDesc = {};
        vkDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        vkDesc.pNext = nullptr,
        vkDesc.setLayoutCount = (uint)m_vkDescriptorSetLayouts.size();
        vkDesc.pSetLayouts = m_vkDescriptorSetLayouts.data();
        vkDesc.pushConstantRangeCount = (uint)vkRootConstants.size();
        vkDesc.pPushConstantRanges = vkRootConstants.data();

        VkDevice & vkDevice = driver::Device::get()->getVulkanDevice();

        VG_ASSERT_VULKAN(vkCreatePipelineLayout(vkDevice, &vkDesc, nullptr, &m_vkPipelineLayout));
    }

    //--------------------------------------------------------------------------------------
    RootSignature::~RootSignature()
    {
        VkDevice & vkDevice = driver::Device::get()->getVulkanDevice();

        for (VkDescriptorSetLayout & vkDescriptorSetLayout : m_vkDescriptorSetLayouts)
            vkDestroyDescriptorSetLayout(vkDevice, vkDescriptorSetLayout, nullptr);
        m_vkDescriptorSetLayouts.clear();

        vkDestroyPipelineLayout(vkDevice, m_vkPipelineLayout, nullptr);
    }

    //--------------------------------------------------------------------------------------
    VkShaderStageFlags RootSignature::getVulkanShaderStageFlags(ShaderStageFlags _shaderStageFlags)
    {
        if (testAllFlags(_shaderStageFlags, ShaderStageFlags::VS))
            return VK_SHADER_STAGE_VERTEX_BIT;
        else if (testAllFlags(_shaderStageFlags, ShaderStageFlags::HS))
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        else if (testAllFlags(_shaderStageFlags, ShaderStageFlags::DS))
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        else if (testAllFlags(_shaderStageFlags, ShaderStageFlags::GS))
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        else if (testAllFlags(_shaderStageFlags, ShaderStageFlags::PS))
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        else if (testAllFlags(_shaderStageFlags, ShaderStageFlags::CS))
            return VK_SHADER_STAGE_COMPUTE_BIT;
        else
            return VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    }
}