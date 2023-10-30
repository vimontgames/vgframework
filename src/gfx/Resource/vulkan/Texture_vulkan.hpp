namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    VkFormat Texture::getVulkanPixelFormat(PixelFormat _format)
    {
        switch (_format)
        {
            default:
                VG_ASSERT(false, "Unhandled pixel format \"%s\"", asString(_format).c_str());
                return VK_FORMAT_UNDEFINED;

            case PixelFormat::R8G8B8A8_unorm:
                return VK_FORMAT_R8G8B8A8_UNORM;

            case PixelFormat::R8G8B8A8_unorm_sRGB:
                return VK_FORMAT_R8G8B8A8_SRGB;

            case PixelFormat::B8G8R8A8_unorm:
                return VK_FORMAT_B8G8R8A8_UNORM;

            case PixelFormat::B8G8R8A8_unorm_sRGB:
                return VK_FORMAT_B8G8R8A8_SRGB;

            case PixelFormat::R16G16B16A16_float:
                return VK_FORMAT_R16G16B16A16_SFLOAT;

            case PixelFormat::D32S8:
                return VK_FORMAT_D32_SFLOAT_S8_UINT;
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
	Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot) :
		base::Texture(_texDesc, _name, _initData)
	{
		auto * device = gfx::Device::get();

		if (_texDesc.isBackbuffer())
		{
            m_resource.setVulkanImage(*(VkImage*)_initData, VK_NULL_HANDLE);
            m_resource.setResourceFlags(ResourceFlags::Backbuffer);
		}
        else
        {
            VkImageCreateInfo imgDesc = {};
                              imgDesc.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                              imgDesc.imageType = getVulkanImageType(_texDesc.type);
                              imgDesc.format = getVulkanPixelFormat(_texDesc.format);
                              imgDesc.extent = { _texDesc.width, _texDesc.height, _texDesc.depth };
                              imgDesc.mipLevels = _texDesc.mipmaps;
                              imgDesc.arrayLayers = 1;
                              imgDesc.samples = VK_SAMPLE_COUNT_1_BIT;
                              imgDesc.flags = 0;
                              

            if (_texDesc.isRenderTarget())
            {
                imgDesc.tiling = VK_IMAGE_TILING_OPTIMAL;
                imgDesc.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                imgDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
            else if (_texDesc.isDepthStencil())
            {
                imgDesc.tiling = VK_IMAGE_TILING_OPTIMAL;
                imgDesc.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                imgDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
            else 
            {
                imgDesc.tiling = VK_IMAGE_TILING_OPTIMAL; // VK_IMAGE_TILING_LINEAR;
                imgDesc.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                imgDesc.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            }

            if (_texDesc.isShaderResource())
                imgDesc.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;

            if (asBool(BindFlags::UnorderedAccess & _texDesc.resource.m_bindFlags))
                imgDesc.usage |= VK_IMAGE_USAGE_STORAGE_BIT;

            VmaAllocationCreateInfo allocCreateInfo = {};
                                    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

            VkImage vkImage;
            VmaAllocation vmaAlloc;
            VG_ASSERT_VULKAN(vmaCreateImage(device->getVulkanMemoryAllocator(), &imgDesc, &allocCreateInfo, &vkImage, &vmaAlloc, nullptr));
            m_resource.setVulkanImage(vkImage, vmaAlloc);
        }

		if (_texDesc.isShaderResource() || _texDesc.isBackbuffer())
		{
            VkImageViewCreateInfo vkImageViewDesc = {};

			vkImageViewDesc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			vkImageViewDesc.pNext = nullptr;
			vkImageViewDesc.format = getVulkanPixelFormat(_texDesc.format);
            vkImageViewDesc.image = m_resource.getVulkanImage();

			vkImageViewDesc.components.r = VK_COMPONENT_SWIZZLE_R;
			vkImageViewDesc.components.g = VK_COMPONENT_SWIZZLE_G;
			vkImageViewDesc.components.b = VK_COMPONENT_SWIZZLE_B;
			vkImageViewDesc.components.a = VK_COMPONENT_SWIZZLE_A;

			vkImageViewDesc.subresourceRange.aspectMask = isDepthStencilFormat(_texDesc.format) ? (VK_IMAGE_ASPECT_DEPTH_BIT/*|VK_IMAGE_ASPECT_STENCIL_BIT*/) : VK_IMAGE_ASPECT_COLOR_BIT; // TODO: create another view for stencil if needed
			vkImageViewDesc.subresourceRange.baseMipLevel = 0;
			vkImageViewDesc.subresourceRange.levelCount = _texDesc.mipmaps;
			vkImageViewDesc.subresourceRange.baseArrayLayer = 0;
			vkImageViewDesc.subresourceRange.layerCount = 1;

			vkImageViewDesc.viewType = getVulkanImageViewType(_texDesc.type);
			vkImageViewDesc.flags = 0;

			VG_ASSERT_VULKAN(vkCreateImageView(device->getVulkanDevice(), &vkImageViewDesc, nullptr, &m_vkImageView));

            if (!_texDesc.isBackbuffer())
            {
                BindlessTable * bindlessTable = device->getBindlessTable();
                m_bindlessSRVHandle = bindlessTable->allocBindlessTextureHandle(static_cast<gfx::Texture*>(this), _reservedSlot);

                VkDescriptorImageInfo tex_descs = {};
                                      tex_descs.imageView = m_vkImageView;
                                      tex_descs.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                      tex_descs.sampler = nullptr;

                VkWriteDescriptorSet writes = {};
                                     writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                     writes.dstBinding = BINDLESS_TEXTURE_SRV_BINDING;
                                     writes.descriptorCount = 1;
                                     writes.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                                     writes.pImageInfo = &tex_descs;
                                     writes.dstSet = device->getVulkanBindlessDescriptors();
                                     writes.dstArrayElement = m_bindlessSRVHandle - BINDLESS_TEXTURE_SRV_START;

                vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);

                if (nullptr != _initData)
                {
                    const auto fmtSize = getPixelFormatSize(_texDesc.format);

                    VkMemoryRequirements mem_reqs;
                    vkGetImageMemoryRequirements(device->getVulkanDevice(), getResource().getVulkanImage(), &mem_reqs);

                    u64 uploadBufferSize = mem_reqs.size;

                    auto * uploadBuffer = device->getUploadBuffer();
                    u8 * dst = uploadBuffer->map(uploadBufferSize, (uint)mem_reqs.alignment);
                    if (nullptr != dst)
                    {
                        uint_ptr currentOffset = 0;
                        for (uint i = 0; i < _texDesc.mipmaps; ++i)
                        {
                            const uint w = _texDesc.width >> i;
                            const uint h = _texDesc.height >> i;

                            // Copy to upload buffer line by line
                            for (uint y = 0; y < h; ++y)
                                memcpy(currentOffset + dst + y * w * fmtSize, currentOffset + &((u8*)_initData)[y * w * fmtSize], fmtSize * w);

                            currentOffset += w * h * fmtSize;
                        }
                    }
                    uploadBuffer->unmap(static_cast<gfx::Texture*>(this), dst);
                }

                if (asBool(BindFlags::UnorderedAccess & _texDesc.resource.m_bindFlags))
                {
                    m_bindlessRWTextureHandle = bindlessTable->allocBindlessRWTextureHandle(static_cast<gfx::Texture *>(this));

                    VkDescriptorImageInfo tex_descs = {};
                    tex_descs.imageView = m_vkImageView;
                    tex_descs.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                    tex_descs.sampler = nullptr;

                    VkWriteDescriptorSet writes = {};
                    writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    writes.dstBinding = BINDLESS_TEXTURE_UAV_BINDING;
                    writes.descriptorCount = 1;
                    writes.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                    writes.pImageInfo = &tex_descs;
                    writes.dstSet = device->getVulkanBindlessDescriptors();
                    writes.dstArrayElement = m_bindlessRWTextureHandle - BINDLESS_TEXTURE_UAV_START;

                    vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);
                }
            }
		}
	}

	//--------------------------------------------------------------------------------------
	Texture::~Texture()
	{
        auto * device = gfx::Device::get();

        // Swapchain must be destroyed using PFN_vkDestroySwapchainKHR
        if (!getTexDesc().isBackbuffer())
		    vkDestroyImageView(device->getVulkanDevice(), m_vkImageView, nullptr);
	}
}