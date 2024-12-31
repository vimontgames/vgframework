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

            case PixelFormat::R10G10B10A2_unorm:
                return VK_FORMAT_A2B10G10R10_UNORM_PACK32;

            case PixelFormat::R16G16B16A16_float:
                return VK_FORMAT_R16G16B16A16_SFLOAT;

            case PixelFormat::R16G16_float:
                return VK_FORMAT_R16G16_SFLOAT;

            case PixelFormat::R32G32B32A32_float:
                return VK_FORMAT_R32G32B32A32_SFLOAT;

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
                VG_ASSERT(false, "Unhandled pixel format %u", _vkFormat);
                return PixelFormat::Unknow;

            case VK_FORMAT_R8G8B8A8_UNORM:
                return PixelFormat::R8G8B8A8_unorm;

            case VK_FORMAT_R8G8B8A8_SRGB:
                return PixelFormat::R8G8B8A8_unorm_sRGB;

            case VK_FORMAT_B8G8R8A8_UNORM:
                return PixelFormat::B8G8R8A8_unorm;

            case VK_FORMAT_B8G8R8A8_SRGB :
                return PixelFormat::B8G8R8A8_unorm_sRGB;

            case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
                return PixelFormat::R10G10B10A2_unorm;

            case VK_FORMAT_R16G16B16A16_SFLOAT:
                return PixelFormat::R16G16B16A16_float;

            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                return PixelFormat::D32S8;
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
                              imgDesc.extent = { _texDesc.width, _texDesc.height, 1 }; // TODO: Texture3D
                              imgDesc.mipLevels = _texDesc.mipmaps;
                              imgDesc.arrayLayers = _texDesc.slices;
                              imgDesc.samples = (VkSampleCountFlagBits)TextureDesc::getMSAASampleCount(_texDesc.msaa);
                              imgDesc.flags = 0;

            if (_texDesc.type == TextureType::TextureCube || _texDesc.type == TextureType::TextureCubeArray)
                imgDesc.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

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
            else if (asBool(BindFlags::UnorderedAccess & _texDesc.resource.m_bindFlags))
            {
                imgDesc.tiling = VK_IMAGE_TILING_OPTIMAL;
                imgDesc.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                imgDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
            else 
            {
                imgDesc.tiling = VK_IMAGE_TILING_OPTIMAL; 
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
            VG_VERIFY_VULKAN(vmaCreateImage(device->getVulkanMemoryAllocator(), &imgDesc, &allocCreateInfo, &vkImage, &vmaAlloc, nullptr));
            m_resource.setVulkanImage(vkImage, vmaAlloc);
        }

        VkImageViewCreateInfo vkImageViewDesc = {};

		if (_texDesc.isShaderResource() || _texDesc.isBackbuffer() || asBool(BindFlags::UnorderedAccess & _texDesc.resource.m_bindFlags))
		{
            auto createVulkanShaderResourceView = [&](const TextureDesc & _texDesc, bool _stencil = false)
            {
                vkImageViewDesc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                vkImageViewDesc.pNext = nullptr;
                vkImageViewDesc.format = getVulkanPixelFormat(_texDesc.format);
                vkImageViewDesc.image = m_resource.getVulkanImage();

                vkImageViewDesc.components.r = VK_COMPONENT_SWIZZLE_R;
                vkImageViewDesc.components.g = VK_COMPONENT_SWIZZLE_G;
                vkImageViewDesc.components.b = VK_COMPONENT_SWIZZLE_B;
                vkImageViewDesc.components.a = VK_COMPONENT_SWIZZLE_A;

                if (isDepthStencilFormat(_texDesc.format))
                {
                    if (_stencil)
                        vkImageViewDesc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
                    else
                        vkImageViewDesc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                }
                else
                {
                    vkImageViewDesc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                }

                vkImageViewDesc.subresourceRange.baseMipLevel = 0;
                vkImageViewDesc.subresourceRange.levelCount = _texDesc.mipmaps;
                vkImageViewDesc.subresourceRange.baseArrayLayer = 0;
                vkImageViewDesc.subresourceRange.layerCount = _texDesc.slices; // TODO: Texture3D

                vkImageViewDesc.viewType = getVulkanImageViewType(_texDesc.type);
                vkImageViewDesc.flags = 0;

                // Can't return a bindless handle for backbuffer, but still need to create an imageView
                VG_VERIFY_VULKAN(vkCreateImageView(device->getVulkanDevice(), &vkImageViewDesc, nullptr, &m_vkImageView));

                if (!_texDesc.isBackbuffer())
                {
                    BindlessTable * bindlessTable = device->getBindlessTable();
                    BindlessTextureHandle srvHandle = bindlessTable->allocBindlessTextureHandle(static_cast<gfx::Texture *>(this), _reservedSlot);

                    VkDescriptorImageInfo tex_descs = {};
                    tex_descs.imageView = m_vkImageView;
                    tex_descs.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    tex_descs.sampler = nullptr;

                    VkWriteDescriptorSet writes = {};
                    writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    writes.dstBinding = BINDLESS_TEXTURE_BINDING;
                    writes.descriptorCount = 1;
                    writes.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                    writes.pImageInfo = &tex_descs;
                    writes.dstSet = device->getVulkanBindlessDescriptors();
                    writes.dstArrayElement = srvHandle - BINDLESS_TEXTURE_START;

                    vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);

                    return srvHandle;
                }
                else
                {
                    return BindlessTextureHandle(BINDLESS_TEXTURE_INVALID);
                }
            };

            if (asBool(TextureFlags::DepthStencil & _texDesc.flags))
            {
                m_depthTextureHandle = createVulkanShaderResourceView(_texDesc, false);
                m_stencilTextureHandle = createVulkanShaderResourceView(_texDesc, true);
            }
            else
            {
                m_textureHandle = createVulkanShaderResourceView(_texDesc);
            }

            if (nullptr != _initData && !_texDesc.isBackbuffer())
            {
                const auto fmtSize = getPixelFormatSize(_texDesc.format);

                VkMemoryRequirements mem_reqs;
                vkGetImageMemoryRequirements(device->getVulkanDevice(), getResource().getVulkanImage(), &mem_reqs);

                u64 uploadBufferSize = mem_reqs.size;
                const auto * scheduler = Kernel::getScheduler();
                VG_ASSERT(scheduler->IsMainThread() || scheduler->IsLoadingThread(), "Expected Main or Loading thread but current thread is \"%s\"", scheduler->GetCurrentThreadName().c_str());
                auto * uploadBuffer = device->getUploadBuffer(0);
                u8 * dst = uploadBuffer->map((gfx::Texture *)this, uploadBufferSize, (uint)mem_reqs.alignment);
                if (nullptr != dst)
                {
                    uint_ptr currentOffset = 0;

                    for (uint s = 0; s < _texDesc.slices; ++s)
                    {
                        for (uint i = 0; i < _texDesc.mipmaps; ++i)
                        {
                            const uint w = max((uint)_texDesc.width >> i, 1U);
                            const uint h = max((uint)_texDesc.height >> i, 1U);

                            // Copy to upload buffer line by line
                            for (uint y = 0; y < h; ++y)
                                memcpy(currentOffset + dst + y * w * fmtSize, currentOffset + &((u8 *)_initData)[y * w * fmtSize], fmtSize * w);

                            currentOffset += w * h * fmtSize;
                        }
                    }
                }
                uploadBuffer->unmap((gfx::Texture*)this, dst, uploadBufferSize);
            }

            if (asBool(BindFlags::UnorderedAccess & _texDesc.resource.m_bindFlags))
            {
                switch (_texDesc.type)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(_texDesc.type);
                        break;

                    case TextureType::TextureCube:
                    {
                        VG_ASSERT(_texDesc.slices == 6);
      
                        m_vkMipImageViews.resize(_texDesc.mipmaps);
                        m_rwTextureMipHandles.resize(_texDesc.mipmaps);

                        for (uint m = 0; m < _texDesc.mipmaps; ++m)
                        {
                            BindlessTable * bindlessTable = device->getBindlessTable();

                            // TODO: batch allocations
                            auto rwTextureHandle = bindlessTable->allocBindlessRWTextureHandle(static_cast<gfx::Texture *>(this));

                            VkImageViewCreateInfo vkMipImageViewDesc = vkImageViewDesc;
                            vkMipImageViewDesc.subresourceRange.baseMipLevel = m;
                            vkMipImageViewDesc.subresourceRange.levelCount = 1;
                            vkMipImageViewDesc.subresourceRange.baseArrayLayer = 0;
                            vkMipImageViewDesc.subresourceRange.layerCount = _texDesc.slices;

                            VG_VERIFY_VULKAN(vkCreateImageView(device->getVulkanDevice(), &vkMipImageViewDesc, nullptr, &m_vkMipImageViews[m]));

                            VkDescriptorImageInfo tex_descs = {};
                            tex_descs.imageView = m_vkMipImageViews[m];
                            tex_descs.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                            tex_descs.sampler = nullptr;

                            VkWriteDescriptorSet writes = {};
                            writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                            writes.dstBinding = BINDLESS_RWTEXTURE_BINDING;
                            writes.descriptorCount = 1;
                            writes.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                            writes.pImageInfo = &tex_descs;
                            writes.dstSet = device->getVulkanBindlessDescriptors();
                            writes.dstArrayElement = rwTextureHandle - BINDLESS_RWTEXTURE_START;

                            // TODO: batch descriptor updates
                            vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);

                            m_rwTextureMipHandles[m] = rwTextureHandle;
                        }
                    }
                    break;

                    case TextureType::Texture2D:
                    {
                        m_vkMipImageViews.resize(_texDesc.mipmaps);
                        m_rwTextureMipHandles.resize(_texDesc.mipmaps);

                        for (uint m = 0; m < _texDesc.mipmaps; ++m)
                        {
                            BindlessTable * bindlessTable = device->getBindlessTable();
                            auto rwTextureHandle = bindlessTable->allocBindlessRWTextureHandle(static_cast<gfx::Texture *>(this));

                            VkImageViewCreateInfo vkMipImageViewDesc = vkImageViewDesc;
                                                  vkMipImageViewDesc.subresourceRange.baseMipLevel = m;
                                                  vkMipImageViewDesc.subresourceRange.levelCount = 1;
                            
                            VG_VERIFY_VULKAN(vkCreateImageView(device->getVulkanDevice(), &vkMipImageViewDesc, nullptr, &m_vkMipImageViews[m]));

                            VkDescriptorImageInfo tex_descs = {};
                            tex_descs.imageView = m_vkMipImageViews[m];
                            tex_descs.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                            tex_descs.sampler = nullptr;

                            VkWriteDescriptorSet writes = {};
                            writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                            writes.dstBinding = BINDLESS_RWTEXTURE_BINDING;
                            writes.descriptorCount = 1;
                            writes.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                            writes.pImageInfo = &tex_descs;
                            writes.dstSet = device->getVulkanBindlessDescriptors();
                            writes.dstArrayElement = rwTextureHandle - BINDLESS_RWTEXTURE_START;

                            vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);

                            m_rwTextureMipHandles[m] = rwTextureHandle;
                        }
                    }
                    break;
                }
            }
        }
	}

	//--------------------------------------------------------------------------------------
	Texture::~Texture()
	{
        auto * device = gfx::Device::get();
        auto & vkDevice = device->getVulkanDevice();

        // Swapchain must be destroyed using PFN_vkDestroySwapchainKHR
        if (!getTexDesc().isBackbuffer())
        {
            vkDestroyImageView(vkDevice, m_vkImageView, nullptr);

            for (uint m = 0; m < m_vkMipImageViews.size(); ++m)
                vkDestroyImageView(vkDevice, m_vkMipImageViews[m], nullptr);
        }
	}
}