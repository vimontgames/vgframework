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
        case VK_FORMAT_R8G8B8A8_UNORM:
            return PixelFormat::R8G8B8A8_unorm;

        case VK_FORMAT_B8G8R8A8_UNORM:
            return PixelFormat::B8G8R8A8_unorm;

        default:
            VG_ASSERT(false, "Unhandled pixel format \"%s\"", asString(_vkFormat).c_str());
            return PixelFormat::Unknow;
        }
    }

	//--------------------------------------------------------------------------------------
	Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData) :
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
                              imgDesc.imageType = VK_IMAGE_TYPE_2D;
                              imgDesc.format = getVulkanPixelFormat(_texDesc.format);
                              imgDesc.extent = { _texDesc.width, _texDesc.height, _texDesc.depth };
                              imgDesc.mipLevels = 1;
                              imgDesc.arrayLayers = 1;
                              imgDesc.samples = VK_SAMPLE_COUNT_1_BIT;
                              imgDesc.tiling = VK_IMAGE_TILING_LINEAR;
                              imgDesc.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
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

			imgViewDesc.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imgViewDesc.flags = 0;

			VG_ASSERT_VULKAN(vkCreateImageView(device->getVulkanDevice(), &imgViewDesc, nullptr, &m_vkImageView));
		}
	}

	//--------------------------------------------------------------------------------------
	Texture::~Texture()
	{
        auto * device = driver::Device::get();
		vkDestroyImageView(device->getVulkanDevice(), m_vkImageView, nullptr);
	}
}