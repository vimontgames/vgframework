namespace vg::graphics::driver::vulkan
{
	//--------------------------------------------------------------------------------------
	RenderPass::RenderPass()
	{

	}

	//--------------------------------------------------------------------------------------
	RenderPass::~RenderPass()
	{

	}

	//--------------------------------------------------------------------------------------
	void RenderPass::finalize()
	{
		for (const driver::Texture * tex : m_attachments)
		{
			VkAttachmentDescription att;
			
			att.flags          = (VkAttachmentDescriptionFlags)0;
			att.format         = Texture::getVulkanPixelFormat(tex->getTexDesc().format);
			att.samples        = VK_SAMPLE_COUNT_1_BIT;
			att.loadOp	       = VK_ATTACHMENT_LOAD_OP_CLEAR;
			att.storeOp	       = VK_ATTACHMENT_STORE_OP_STORE;
			att.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			att.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
			att.finalLayout	   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			m_vkAttachmentDescriptions.push_back(att);
		}

        for (SubPass * subPass : m_subPasses)
        {
            VkSubpassDescription vkSubPass;

            const VkAttachmentReference colorAttRef =  {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

            vkSubPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            vkSubPass.flags = 0;
            vkSubPass.inputAttachmentCount = 0;
            vkSubPass.pInputAttachments = nullptr;
            vkSubPass.colorAttachmentCount = 1;
            vkSubPass.pColorAttachments = &colorAttRef;
            vkSubPass.pResolveAttachments = nullptr;
            vkSubPass.pDepthStencilAttachment = nullptr;
            vkSubPass.preserveAttachmentCount = 0;
            vkSubPass.pPreserveAttachments = nullptr;

            m_vkSubPasses.push_back(vkSubPass);

            // keep copy for debug (?)
            subPass->m_vkSubPass = vkSubPass;
        }

        VkRenderPassCreateInfo vkRenderPassDesc; 
        vkRenderPassDesc.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        vkRenderPassDesc.pNext = nullptr;
        vkRenderPassDesc.flags = 0;
        vkRenderPassDesc.attachmentCount = (uint)m_vkAttachmentDescriptions.size();
        vkRenderPassDesc.pAttachments = m_vkAttachmentDescriptions.data();
        vkRenderPassDesc.subpassCount = (uint)m_vkSubPasses.size();
        vkRenderPassDesc.pSubpasses = m_vkSubPasses.data();
        vkRenderPassDesc.dependencyCount = 0;
        vkRenderPassDesc.pDependencies = nullptr;

        VkDevice & vkDevice = driver::Device::get()->getVulkanDevice();

        VG_ASSERT_VULKAN(vkCreateRenderPass(vkDevice, &vkRenderPassDesc, nullptr, &m_vkRenderPass));

        core::vector<VkImageView> vkImageViews;
        for (const driver::Texture * tex : m_attachments)
        {
            vkImageViews.push_back(tex->m_vkImageView);
        }

        VkFramebufferCreateInfo frameBufferInfo;
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.flags = 0;
        frameBufferInfo.pNext = nullptr;
        frameBufferInfo.renderPass = m_vkRenderPass;
        frameBufferInfo.attachmentCount = (uint)vkImageViews.size();
        frameBufferInfo.pAttachments = vkImageViews.data();
        frameBufferInfo.width = 1280;
        frameBufferInfo.height = 720;
        frameBufferInfo.layers = 1;

        VG_ASSERT_VULKAN(vkCreateFramebuffer(vkDevice, &frameBufferInfo, nullptr, &m_vkFrameBuffer));

        VkRenderPassBeginInfo & vkRenderPassBeginInfo = m_vkRenderPassBeginInfo;

        const auto & clearColor = m_subPasses[0]->m_userPasses[0]->getRenderTargets()[0]->getTextureDesc().clearColor;
  
        vkRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        vkRenderPassBeginInfo.pNext = nullptr;
        vkRenderPassBeginInfo.renderPass = m_vkRenderPass;
        vkRenderPassBeginInfo.framebuffer = m_vkFrameBuffer;
        vkRenderPassBeginInfo.renderArea.offset.x = 0;
        vkRenderPassBeginInfo.renderArea.offset.y = 0;
        vkRenderPassBeginInfo.renderArea.extent.width = 1280;
        vkRenderPassBeginInfo.renderArea.extent.height = 720;
        vkRenderPassBeginInfo.clearValueCount = 1;
        vkRenderPassBeginInfo.pClearValues = (const VkClearValue*)&clearColor;
	}
}