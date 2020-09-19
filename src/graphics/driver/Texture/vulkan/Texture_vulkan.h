#pragma once

#include "graphics/driver/Texture/Texture.h"

class vkDevice;

namespace vg::graphics::driver::vulkan
{
	class Texture : public base::Texture
	{
		using super = base::Texture;

	public:
        static VkFormat     getVulkanPixelFormat    (PixelFormat _format);
        static PixelFormat  getPixelFormat          (VkFormat _vkFormat);

		Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData);
		~Texture();

	//private:
		VkImage				m_vkImage;
        VmaAllocation       m_vma_alloc;
		VkImageView			m_vkImageView;
	};
}