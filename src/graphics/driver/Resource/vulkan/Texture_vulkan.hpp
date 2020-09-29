namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    VkFormat Texture::getVulkanPixelFormat(PixelFormat _format)
    {
        switch (_format)
        {
        case PixelFormat::R8G8B8A8_unorm:
            return VK_FORMAT_R8G8B8A8_UNORM;

        case PixelFormat::B8G8R8A8_unorm:
            return VK_FORMAT_B8G8R8A8_UNORM;

        default:
            VG_ASSERT(false, "Unhandled pixel format \"%s\"", asString(_format).c_str());
            return VK_FORMAT_UNDEFINED;
        }
    }

    //--------------------------------------------------------------------------------------
    PixelFormat Texture::getPixelFormat(VkFormat _vkFormat)
    {
        switch (_vkFormat)
        {
            default:
                VG_ASSERT(false, "Unhandled pixel format \"%s\"", asString(_vkFormat).c_str());
                return PixelFormat::Unknow;

            case VK_FORMAT_R8G8B8A8_UNORM:
                return PixelFormat::R8G8B8A8_unorm;

            case VK_FORMAT_B8G8R8A8_UNORM:
                return PixelFormat::B8G8R8A8_unorm;
        }
    }

    //--------------------------------------------------------------------------------------
    VkImageType Texture::getVulkanImageType(TextureType _texType)
    {
        switch (_texType)
        {
            default:
                VG_ASSERT(false, "Unhandled TextureType \"%s\"", asString(_texType).c_str());

            case TextureType::Texture1D:
            case TextureType::Texture1DArray:
                return VK_IMAGE_TYPE_1D;

            case TextureType::Texture2D:
            case TextureType::Texture2DArray:
            case TextureType::Texture2DMS:
            case TextureType::Texture2DMSArray:
            case TextureType::TextureCube:
            case TextureType::TextureCubeArray:
                return VK_IMAGE_TYPE_2D;

            case TextureType::Texture3D:
                return VK_IMAGE_TYPE_3D;
        }
    }

    //--------------------------------------------------------------------------------------
    VkImageViewType Texture::getVulkanImageViewType(TextureType _texType)
    {
        switch (_texType)
        {
            default:
                VG_ASSERT(false, "Unhandled TextureType \"%s\"", asString(_texType).c_str());

            case TextureType::Texture1D:
                return VK_IMAGE_VIEW_TYPE_1D;

            case TextureType::Texture1DArray:
                return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

            case TextureType::Texture2D:
            case TextureType::Texture2DMS:
                return VK_IMAGE_VIEW_TYPE_2D;

            case TextureType::Texture2DArray:
            case TextureType::Texture2DMSArray:
                return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

            case TextureType::TextureCube:
                return VK_IMAGE_VIEW_TYPE_CUBE;

            case TextureType::TextureCubeArray:
                return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

            case TextureType::Texture3D:
                return VK_IMAGE_VIEW_TYPE_3D;
        }
    }

	//--------------------------------------------------------------------------------------
	Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData, ReservedSlot _reservedSlot) :
		base::Texture(_texDesc, _name, _initData)
	{
		auto * device = driver::Device::get();

		if (asBool(TextureFlags::Backbuffer & _texDesc.flags))
		{
            m_resource.setVulkanImage(*static_cast<VkImage*>(_initData), nullptr);
		}
        else
        {
            VkImageCreateInfo imgDesc = {};
                              imgDesc.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                              imgDesc.imageType = getVulkanImageType(_texDesc.type);
                              imgDesc.format = getVulkanPixelFormat(_texDesc.format);
                              imgDesc.extent = { _texDesc.width, _texDesc.height, _texDesc.depth };
                              imgDesc.mipLevels = 1;
                              imgDesc.arrayLayers = 1;
                              imgDesc.samples = VK_SAMPLE_COUNT_1_BIT;
                              imgDesc.tiling = VK_IMAGE_TILING_LINEAR;
                              imgDesc.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                              imgDesc.flags = 0;
                              imgDesc.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

            VmaAllocationCreateInfo allocCreateInfo = {};
                                    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

            VkImage vkImage;
            VmaAllocation vmaAlloc;
            VG_ASSERT_VULKAN(vmaCreateImage(device->getVulkanMemoryAllocator(), &imgDesc, &allocCreateInfo, &vkImage, &vmaAlloc, nullptr));
            m_resource.setVulkanImage(vkImage, vmaAlloc);
        }

		if (asBool(BindFlags::ShaderResource & _texDesc.resource.m_bindFlags) || asBool(TextureFlags::Backbuffer & _texDesc.flags))
		{
			VkImageViewCreateInfo imgViewDesc;

			imgViewDesc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imgViewDesc.pNext = nullptr;
			imgViewDesc.format = getVulkanPixelFormat(_texDesc.format);
            imgViewDesc.image = m_resource.getVulkanImage();

			imgViewDesc.components.r = VK_COMPONENT_SWIZZLE_R;
			imgViewDesc.components.g = VK_COMPONENT_SWIZZLE_G;
			imgViewDesc.components.b = VK_COMPONENT_SWIZZLE_B;
			imgViewDesc.components.a = VK_COMPONENT_SWIZZLE_A;

			imgViewDesc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imgViewDesc.subresourceRange.baseMipLevel = 0;
			imgViewDesc.subresourceRange.levelCount = 1;
			imgViewDesc.subresourceRange.baseArrayLayer = 0;
			imgViewDesc.subresourceRange.layerCount = 1;

			imgViewDesc.viewType = getVulkanImageViewType(_texDesc.type);
			imgViewDesc.flags = 0;

			VG_ASSERT_VULKAN(vkCreateImageView(device->getVulkanDevice(), &imgViewDesc, nullptr, &m_vkImageView));

            if (!asBool(TextureFlags::Backbuffer & _texDesc.flags))
            {
                BindlessTable * bindlessTable = device->getBindlessTable();
                m_bindlessTextureHandle = bindlessTable->allocBindlessTextureHandle(static_cast<driver::Texture*>(this), _reservedSlot);

                VkDescriptorImageInfo tex_descs = {};
                tex_descs.imageView = m_vkImageView;
                tex_descs.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                tex_descs.sampler = nullptr; // device->vk_immutableSampler;

                VkWriteDescriptorSet writes = {};
                writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes.dstBinding = 0; // register
                writes.descriptorCount = 1;
                writes.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                writes.pImageInfo = &tex_descs;
                writes.dstSet = device->m_vkbindlessDescriptorSet[0];
                writes.dstArrayElement = m_bindlessTextureHandle; 

                vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);

                // should be done once @ init time, not for every texture !
                VkDescriptorImageInfo samp_descs = {};
                samp_descs.imageView = VK_NULL_HANDLE;
                samp_descs.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                samp_descs.sampler = device->vk_immutableSampler;
                
                VkWriteDescriptorSet writeSamplers = {};
                writeSamplers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeSamplers.dstBinding = 0; // register
                writeSamplers.descriptorCount = 1;
                writeSamplers.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                writeSamplers.pImageInfo = &samp_descs;
                writeSamplers.dstSet = device->m_vkbindlessDescriptorSet[1];
                writeSamplers.dstArrayElement = 0; // offset
                
                vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writeSamplers, 0, nullptr);

                if (nullptr != _initData)
                {
                    const auto fmtSize = getPixelFormatSize(_texDesc.format);

                    VkMemoryRequirements mem_reqs;
                    vkGetImageMemoryRequirements(device->getVulkanDevice(), getResource().getVulkanImage(), &mem_reqs);

                    u64 uploadBufferSize = mem_reqs.size;

                    auto & context = device->getCurrentFrameContext();

                    auto * mapped = context.m_uploadBuffer->getResource().m_vmaAllocInfo.pMappedData;

                    // Copy to upload buffer line by line
                    context.m_uploadCur = (u8*)alignUp((uint_ptr)context.m_uploadCur, (core::u32)mem_reqs.alignment);
                    const u32 offset = (u32)(context.m_uploadCur - context.m_uploadBegin);

                    for (uint y = 0; y < _texDesc.height; ++y)
                        memcpy(context.m_uploadCur + y * _texDesc.width * fmtSize, &((u8*)_initData)[y * _texDesc.width * fmtSize], fmtSize * _texDesc.width);

                    device->upload(static_cast<driver::Texture*>(this), offset);

                    context.m_uploadCur += uploadBufferSize;
                }
            }
		}
	}

	//--------------------------------------------------------------------------------------
	Texture::~Texture()
	{
        auto * device = driver::Device::get();
		vkDestroyImageView(device->getVulkanDevice(), m_vkImageView, nullptr);
	}
}