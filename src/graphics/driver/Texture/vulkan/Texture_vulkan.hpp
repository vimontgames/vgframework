namespace vg::graphics::driver::vulkan
{
	//--------------------------------------------------------------------------------------
	Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData) :
		base::Texture(_texDesc, _name, _initData)
	{
		auto * device = driver::Device::get();

		if (asBool(TextureFlags::Backbuffer & _texDesc.flags))
		{
			m_vkImage = *static_cast<VkImage*>(_initData);
		}

		//if (asBool(TextureFlags::ShaderResource & _texDesc.flags))
		{
			VkImageViewCreateInfo imgViewDesc;

			imgViewDesc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imgViewDesc.pNext = nullptr;
			imgViewDesc.format = VK_FORMAT_R8G8B8A8_UNORM;
            imgViewDesc.image = m_vkImage;

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
		auto & vkDevice = driver::Device::get()->getVulkanDevice();

		vkDestroyImageView(vkDevice, m_vkImageView, nullptr);
		vkDestroyImage(vkDevice, m_vkImage, nullptr);
	}
}