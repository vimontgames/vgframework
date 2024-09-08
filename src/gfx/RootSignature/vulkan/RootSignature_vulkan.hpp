namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    RootSignature::RootSignature(const RootSignatureDesc & _desc) :
        super::RootSignature(_desc)
    {
        gfx::Device * device = gfx::Device::get();
        VkDevice & vkDevice = device->getVulkanDevice();

        core::vector<VkPushConstantRange> vkRootConstants;
        const auto & rootConstants = _desc.getRootConstants();
        for (uint i = 0; i < rootConstants.size(); ++i)
        {
            const RootSignatureDesc::PushConstantParams & rootConstant = rootConstants[i];

            VkPushConstantRange vkRootConstant;

            vkRootConstant.offset = rootConstant.m_register;
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
                    default:
                    VG_ASSERT(false, "Unhandled Descriptor::Type \"%s\" (%u)", asString(descriptor.getDescriptorType()).c_str(), descriptor.getDescriptorType());
                    break;

                    case RootSignatureDescType::ConstantBuffer:
                    {
                        const auto & constantbuffers = descriptor.getConstantBuffers();
                        vkLayoutBinding.binding = constantbuffers.m_binding;
                        vkLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                         vkLayoutBinding.descriptorCount = constantbuffers.m_count;
                        vkLayoutBinding.stageFlags = getVulkanShaderStageFlags(table.getShaderStageFlags());
                        vkLayoutBinding.pImmutableSamplers = nullptr;
                    }
                    break;

                    case RootSignatureDescType::Texture:
                    {
                        const auto & textures = descriptor.getTextures();
                        vkLayoutBinding.binding = textures.m_binding;
                        vkLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                        vkLayoutBinding.descriptorCount = textures.m_count;
                        vkLayoutBinding.stageFlags = getVulkanShaderStageFlags(table.getShaderStageFlags());
                        vkLayoutBinding.pImmutableSamplers = nullptr;
                    }
                    break;

                    case RootSignatureDescType::Buffer:
                    {
                        const auto & buffers = descriptor.getBuffers();
                        vkLayoutBinding.binding = buffers.m_binding;
                        vkLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                        vkLayoutBinding.descriptorCount = buffers.m_count;
                        vkLayoutBinding.stageFlags = getVulkanShaderStageFlags(table.getShaderStageFlags());
                        vkLayoutBinding.pImmutableSamplers = nullptr; 
                    }
                    break;

                    case RootSignatureDescType::RWTexture:
                    {
                        const auto & textures = descriptor.getRWTextures();
                        vkLayoutBinding.binding = textures.m_binding;
                        vkLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                        vkLayoutBinding.descriptorCount = textures.m_count;
                        vkLayoutBinding.stageFlags = getVulkanShaderStageFlags(table.getShaderStageFlags());
                        vkLayoutBinding.pImmutableSamplers = nullptr;
                    }
                    break;

                    case RootSignatureDescType::RWBuffer:
                    {
                        const auto & buffers = descriptor.getRWBuffers();
                        vkLayoutBinding.binding = buffers.m_binding;
                        vkLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                        vkLayoutBinding.descriptorCount = buffers.m_count;
                        vkLayoutBinding.stageFlags = getVulkanShaderStageFlags(table.getShaderStageFlags());
                        vkLayoutBinding.pImmutableSamplers = nullptr;
                    }
                    break;

                    case RootSignatureDescType::TLAS:
                    {
                        const auto & tlas = descriptor.getTLAS();
                        vkLayoutBinding.binding = tlas.m_binding;
                        vkLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
                        vkLayoutBinding.descriptorCount = tlas.m_count;
                        vkLayoutBinding.stageFlags = getVulkanShaderStageFlags(table.getShaderStageFlags());
                        vkLayoutBinding.pImmutableSamplers = nullptr;
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
            vkDescriptorSetLayoutDesc.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

            vector<VkDescriptorBindingFlags> flagsArray;
            for (uint i = 0; i < vkDescriptorSetLayoutBindings.size(); ++i)
            {
                VkDescriptorBindingFlags flags = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
                flagsArray.push_back(flags);
            }

            VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags{};
            binding_flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
            binding_flags.bindingCount = (uint)flagsArray.size();
            binding_flags.pBindingFlags = flagsArray.data();

            vkDescriptorSetLayoutDesc.pNext = &binding_flags;

            VkDescriptorSetLayout vkDescriptorSetLayout;
            VG_VERIFY_VULKAN(vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutDesc, nullptr, &vkDescriptorSetLayout));
            m_vkDescriptorSetLayouts.push_back(vkDescriptorSetLayout);
        }

        // Add separate table for samplers
        {
            VkDescriptorSetLayoutBinding vkLayoutBinding = {};
            vkLayoutBinding.binding = 0;
            vkLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            vkLayoutBinding.descriptorCount = (uint)enumCount<Sampler>(),
            vkLayoutBinding.stageFlags = getVulkanShaderStageFlags(ShaderStageFlags::All);
            vkDescriptorSetLayoutBindings.push_back(vkLayoutBinding);

            VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutDesc = {};
            vkDescriptorSetLayoutDesc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            vkDescriptorSetLayoutDesc.bindingCount = 1;
            vkDescriptorSetLayoutDesc.pBindings = &vkLayoutBinding;

            VkDescriptorSetLayout vkSamplerDescriptorSetLayout;
            VG_VERIFY_VULKAN(vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutDesc, nullptr, &vkSamplerDescriptorSetLayout));
            m_vkDescriptorSetLayouts.push_back(vkSamplerDescriptorSetLayout);
        }

        VkPipelineLayoutCreateInfo vkDesc = {};
        vkDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        vkDesc.pNext = nullptr,
        vkDesc.setLayoutCount = (uint)m_vkDescriptorSetLayouts.size();
        vkDesc.pSetLayouts = m_vkDescriptorSetLayouts.data();
        vkDesc.pushConstantRangeCount = (uint)vkRootConstants.size();
        vkDesc.pPushConstantRanges = vkRootConstants.data();

        VG_VERIFY_VULKAN(vkCreatePipelineLayout(vkDevice, &vkDesc, nullptr, &m_vkPipelineLayout));
    }

    //--------------------------------------------------------------------------------------
    RootSignature::~RootSignature()
    {
        VkDevice & vkDevice = gfx::Device::get()->getVulkanDevice();

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