#pragma once

#include "graphics/driver/FrameGraph/RenderPass.h"

namespace vg::graphics::driver::vulkan
{
	class RenderPass : public base::RenderPass
	{
		using super = base::RenderPass;

	public:
		RenderPass(const RenderPassKey & _key);
		~RenderPass();

		void finalize();

        const VkRenderPassBeginInfo & getVulkanRenderPassBeginInfo() const { return m_vkRenderPassBeginInfo; }

        static VkRenderPass createVulkanRenderPassFromKey(const RenderPassKey & _key);

	private:
        VkFramebuffer                           m_vkFrameBuffer;
        VkRenderPass 							m_vkRenderPass = nullptr;
        VkRenderPassBeginInfo					m_vkRenderPassBeginInfo;
        core::vector<VkSubpassDescription>      m_vkSubPasses;
	};
}