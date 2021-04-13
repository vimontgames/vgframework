#pragma once

#include "graphics/driver/Resource/Texture.h"

class vkDevice;

namespace vg::graphics::driver::vulkan
{
	class Texture : public base::Texture
	{
		using super = base::Texture;

	public:
        static VkFormat         getVulkanPixelFormat    (PixelFormat _format);
        static PixelFormat      getPixelFormat          (VkFormat _vkFormat);
        static VkImageType      getVulkanImageType(TextureType _texType);
        static VkImageViewType  getVulkanImageViewType(TextureType _texType);

		Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot);
		~Texture();

        VkImageView             getVulkanImageView() const { return m_vkImageView; }

	private:
		VkImageView			    m_vkImageView;
	};
}