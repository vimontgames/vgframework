#pragma once

#include "gfx/FrameGraph/RenderPass.h"

namespace vg::gfx::vulkan
{
	class RenderPass : public base::RenderPass
	{
		using super = base::RenderPass;

	public:
        static VkRenderPass createVulkanRenderPassFromKey(const RenderPassKey & _key);

		RenderPass(const RenderPassKey & _key);
		~RenderPass();

		void finalize();
        void begin(CommandList * _cmdList);
        void end(CommandList * _cmdList);

    private:
        static bool createVulkanAttachmentDesc(PixelFormat _format, const SubPassKey::AttachmentInfo & _info, VkAttachmentDescription * _att);

	private:
        VkFramebufferCreateInfo                 m_vkFrameBufferInfo;
        VkFramebuffer                           m_vkFrameBuffer;
        VkRenderPass 							m_vkRenderPass = nullptr;
        VkRenderPassBeginInfo					m_vkRenderPassBeginInfo;
        core::vector<VkSubpassDescription>      m_vkSubPasses;
        core::vector<VkClearValue>              m_vkClearValues;
	};
}