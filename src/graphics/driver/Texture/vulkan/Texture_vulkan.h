#pragma once

#include "graphics/driver/Texture/Texture.h"

class vkDevice;

namespace vg::graphics::driver::vulkan
{
	class Texture : public base::Texture
	{
		using super = base::Texture;

	public:
		Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData);
		~Texture();

	//private:
		VkImage				m_vkImage;
		VkImageView			m_vkImageView;
	};
}