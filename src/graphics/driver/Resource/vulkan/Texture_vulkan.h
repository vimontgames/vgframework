#pragma once

#include "graphics/driver/Resource/Texture.h"

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

        VkImageView         getVulkanImageView() const { return m_vkImageView; }

	private:
		VkImageView			m_vkImageView;
	};
}