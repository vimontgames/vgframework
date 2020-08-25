#pragma once

#include "graphics/driver/FrameGraph/SubPass.h"

namespace vg::graphics::driver::vulkan
{
	class SubPass : public base::SubPass
	{
		using super = base::RenderPass;

	public:
		SubPass();
		~SubPass();

        VkSubpassDescription    m_vkSubPass;
	};
}