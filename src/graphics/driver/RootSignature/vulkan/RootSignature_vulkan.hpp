namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    RootSignature::RootSignature(const RootSignatureDesc & _desc) :
        super::RootSignature(_desc)
    {
        driver::Device * device = driver::Device::get();
        VkDevice & vkDevice = device->getVulkanDevice();

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

        vector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings;
        const auto & tables = _desc.getTables();

        for (uint i = 0; i < tables.size(); ++i)
        {
            const RootSignatureTableDesc & table = tables[i];
            const auto & descriptors = table.getDescriptors();

            for (uint j = 0; j < descriptors.size(); ++j)
            {
                const RootSignatureTableDesc::Descriptor & descriptor = descriptors[j];
                    
                VkDescriptorSetLayoutBinding vkLayoutBinding = {};

                switch (descriptor.getDescriptorType())
                {
                    case RootSignatureTableDesc::Descriptor::Type::Texture:
                    {
                        const auto textures = descriptor.getTextures();
                        vkLayoutBinding.binding = textures.m_register;
                        vkLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                        vkLayoutBinding.descriptorCount = textures.m_count;
                        vkLayoutBinding.stageFlags = getVulkanShaderStageFlags(table.getShaderStageFlags());
                        vkLayoutBinding.pImmutableSamplers = nullptr; // &device->vk_immutableSampler;
                    }
                    break;
                }

                vkDescriptorSetLayoutBindings.push_back(vkLayoutBinding);
            }
        }

        if (vkDescriptorSetLayoutBindings.size() > 0)
        {
            VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutDesc = {};
            vkDescriptorSetLayoutDesc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            vkDescriptorSetLayoutDesc.bindingCount = (uint)vkDescriptorSetLayoutBindings.size();
            vkDescriptorSetLayoutDesc.pBindings = vkDescriptorSetLayoutBindings.data();

            VkDescriptorSetLayout vkDescriptorSetLayout;
            VG_ASSERT_VULKAN(vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutDesc, nullptr, &vkDescriptorSetLayout));
            m_vkDescriptorSetLayouts.push_back(vkDescriptorSetLayout);
        }

        // Add separate table for samplers
        {
            VkDescriptorSetLayoutBinding vkLayoutBinding = {};
            vkLayoutBinding.binding = 0;
            vkLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            vkLayoutBinding.descriptorCount = 1,
            vkLayoutBinding.stageFlags = getVulkanShaderStageFlags(ShaderStageFlags::PS);
            vkDescriptorSetLayoutBindings.push_back(vkLayoutBinding);

            VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutDesc = {};
            vkDescriptorSetLayoutDesc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            vkDescriptorSetLayoutDesc.bindingCount = 1;
            vkDescriptorSetLayoutDesc.pBindings = &vkLayoutBinding;

            VkDescriptorSetLayout vkDescriptorSetLayout;
            VG_ASSERT_VULKAN(vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutDesc, nullptr, &vkDescriptorSetLayout));
            m_vkDescriptorSetLayouts.push_back(vkDescriptorSetLayout);
        }

        VkPipelineLayoutCreateInfo vkDesc = {};
        vkDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        vkDesc.pNext = nullptr,
        vkDesc.setLayoutCount = (uint)m_vkDescriptorSetLayouts.size();
        vkDesc.pSetLayouts = m_vkDescriptorSetLayouts.data();
        vkDesc.pushConstantRangeCount = (uint)vkRootConstants.size();
        vkDesc.pPushConstantRanges = vkRootConstants.data();

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
        VkShaderStageFlags flags = 0;

        if (asBool(ShaderStageFlags::VS & _shaderStageFlags))
            flags|= VK_SHADER_STAGE_VERTEX_BIT;

        if (asBool(ShaderStageFlags::HS & _shaderStageFlags))
            flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

        if (asBool(ShaderStageFlags::DS & _shaderStageFlags))
            flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

        if (asBool(ShaderStageFlags::GS & _shaderStageFlags))
            flags |= VK_SHADER_STAGE_GEOMETRY_BIT;

        if (asBool(ShaderStageFlags::PS & _shaderStageFlags))
            flags |= VK_SHADER_STAGE_FRAGMENT_BIT;

        if (asBool(ShaderStageFlags::CS & _shaderStageFlags))
            flags |= VK_SHADER_STAGE_COMPUTE_BIT;

        return flags;
      }
}