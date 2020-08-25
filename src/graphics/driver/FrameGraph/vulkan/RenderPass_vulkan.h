#pragma once

#include "graphics/driver/FrameGraph/RenderPass.h"

namespace vg::graphics::driver::vulkan
{
	class RenderPass : public base::RenderPass
	{
		using super = base::RenderPass;

	public:
		RenderPass();
		~RenderPass();

		void finalize();

	//private:
        VkFramebuffer                           m_vkFrameBuffer;
        VkRenderPass							m_vkRenderPass;
        VkRenderPassBeginInfo					m_vkRenderPassBeginInfo;
		core::vector<VkAttachmentDescription>	m_vkAttachmentDescriptions;
        core::vector<VkSubpassDescription>      m_vkSubPasses;
	};
}