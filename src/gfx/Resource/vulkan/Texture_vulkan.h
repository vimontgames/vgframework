#pragma once
#include "gfx/Resource/Texture.h"

namespace vg::gfx::vulkan
{
	class Texture : public base::Texture
	{
	public:
		VG_CLASS_DECL_PASSTHROUGH(Texture, base::Texture);

		Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot);
		~Texture();

        VkImageView					getVulkanImageView		() const { return m_vkImageView; }

		static VkFormat				getVulkanPixelFormat    (PixelFormat _format);
        static PixelFormat			getPixelFormat          (VkFormat _vkFormat);
        static VkImageType			getVulkanImageType		(TextureType _texType);
        static VkImageViewType		getVulkanImageViewType	(TextureType _texType);

	private:
		VkImageView					m_vkImageView;		// This is the default image view with all slices/mipmaps (used for ShaderResourceView)
		core::vector<VkImageView>	m_vkMipImageViews;	// Separate image view for each mip level (e.g. used for RWTextures)
	};
}