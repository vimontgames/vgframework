namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    VkSamplerCreateInfo SamplerState::getVulkanSamplerState(Sampler _sampler)
    {
        const driver::SamplerState samplerState(_sampler);

        VkSamplerCreateInfo samplerCreateInfo = {};
                            samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                            samplerCreateInfo.magFilter = getVulkanFilter(samplerState.filter);             // VK_FILTER_NEAREST;
                            samplerCreateInfo.minFilter = getVulkanFilter(samplerState.filter);             // VK_FILTER_NEAREST;
                            samplerCreateInfo.mipmapMode = getVulkanMipmapMode(samplerState.filter);        // VK_SAMPLER_MIPMAP_MODE_NEAREST;
                            samplerCreateInfo.addressModeU = getVulkanAddressMode(samplerState.address);    // VK_SAMPLER_ADDRESS_MODE_REPEAT;
                            samplerCreateInfo.addressModeV = getVulkanAddressMode(samplerState.address);    // VK_SAMPLER_ADDRESS_MODE_REPEAT;
                            samplerCreateInfo.addressModeW = getVulkanAddressMode(samplerState.address);    // VK_SAMPLER_ADDRESS_MODE_REPEAT;
                            samplerCreateInfo.mipLodBias = 0.0;
                            samplerCreateInfo.anisotropyEnable = VK_FALSE;
                            samplerCreateInfo.maxAnisotropy = 1;
                            samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
                            samplerCreateInfo.minLod = 0.0f;
                            samplerCreateInfo.maxLod = VK_LOD_CLAMP_NONE;
                            samplerCreateInfo.compareEnable = VK_FALSE;
                            samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        return samplerCreateInfo;
    }

    //--------------------------------------------------------------------------------------
    VkFilter SamplerState::getVulkanFilter(Filter _filter)
    {
        switch (_filter)
        {
            default:
                VG_ASSERT(false, "Unhandled Filter \"%s\" (%u)", asString(_filter).c_str(), _filter);
            case Filter::Nearest:
                return VK_FILTER_NEAREST;
            case Filter::Linear:
                return VK_FILTER_LINEAR;
        }
    }

    //--------------------------------------------------------------------------------------
    VkSamplerMipmapMode SamplerState::getVulkanMipmapMode(Filter _filter)
    {
        switch (_filter)
        {
            default:
                VG_ASSERT(false, "Unhandled Filter \"%s\" (%u)", asString(_filter).c_str(), _filter);
            case Filter::Nearest:
                return VK_SAMPLER_MIPMAP_MODE_NEAREST;
            case Filter::Linear:
                return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
    }

    //--------------------------------------------------------------------------------------
    VkSamplerAddressMode SamplerState::getVulkanAddressMode(Address _address)
    {
        switch (_address)
        {
            default:
                VG_ASSERT(false, "Unhandled Address \"%s\" (%u)", asString(_address).c_str(), _address);
            case Address::Clamp:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case Address::Repeat:
                return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case Address::Mirror:
                return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        }
    }
}