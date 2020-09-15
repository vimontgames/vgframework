namespace vg::graphics::driver::vulkan
{
	//--------------------------------------------------------------------------------------
	RenderPass::RenderPass(const RenderPassKey & _key) :
        super::RenderPass(_key)
	{

	}

	//--------------------------------------------------------------------------------------
	RenderPass::~RenderPass()
	{
        driver::Device * device = driver::Device::get();
        VkDevice & vkDevice = driver::Device::get()->getVulkanDevice();
        
        //vkDestroyRenderPass(vkDevice, m_vkRenderPass, nullptr);
        device->releaseVulkanRenderPass(m_renderPassKey, m_vkRenderPass);

        vkDestroyFramebuffer(vkDevice, m_vkFrameBuffer, nullptr);
	}

    //--------------------------------------------------------------------------------------
    VkRenderPass RenderPass::createVulkanRenderPassFromKey(const RenderPassKey & _key)
    {
        // Add all attachments
        core::vector<VkAttachmentDescription> vkAttachmentDescriptions;

        for (uint i = 0; i < maxRenderTarget; ++i)
        {
            const auto format = _key.m_colorFormat[i];
            if (PixelFormat::Unknow != format)
            {
                VkAttachmentDescription att;

                att.flags = (VkAttachmentDescriptionFlags)0;
                att.format = Texture::getVulkanPixelFormat(format);

                // TODO: RenderPassKey flags for initial/final state for each attachment
                att.samples = VK_SAMPLE_COUNT_1_BIT;
                att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                att.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                vkAttachmentDescriptions.push_back(att);
            }
        }

        // Add subpass descriptions
        vector<VkSubpassDescription> vkSubPasses;

        // Foreach subpass we need to say the attachment used
        vector<vector<VkAttachmentReference>> vkAttachmentRefs;
        vkAttachmentRefs.resize(_key.m_subPassCount);

        for (uint i = 0; i < _key.m_subPassCount; ++i)
        {
            const auto subPassKey = _key.m_subPassKeys[i];

            VkSubpassDescription vkSubPass = {};
            vector<VkAttachmentReference> & vkSubPassAttachmentRefs = vkAttachmentRefs[i];
            
            for (uint j = 0; j < maxRenderTarget; ++j) // maxRenderTarget == maxAttachment per subPass ?
            {
                const auto flags = subPassKey.getRenderTargetFlags(j);

                if (SubPassKey::Flags::Bind & flags)
                {
                    VkAttachmentReference colorAttRef = { i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
                    vkSubPassAttachmentRefs.push_back(colorAttRef);
                }
            }

            vkSubPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            vkSubPass.flags = 0;
            vkSubPass.inputAttachmentCount = 0;
            vkSubPass.pInputAttachments = nullptr;
            vkSubPass.colorAttachmentCount = (uint)vkSubPassAttachmentRefs.size();
            vkSubPass.pColorAttachments = vkSubPassAttachmentRefs.data();
            vkSubPass.pResolveAttachments = nullptr;
            vkSubPass.pDepthStencilAttachment = nullptr;
            vkSubPass.preserveAttachmentCount = 0;
            vkSubPass.pPreserveAttachments = nullptr;

            vkSubPasses.push_back(vkSubPass);
        }

        VkRenderPassCreateInfo vkRenderPassDesc;
        vkRenderPassDesc.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        vkRenderPassDesc.pNext = nullptr;
        vkRenderPassDesc.flags = 0;
        vkRenderPassDesc.attachmentCount = (uint)vkAttachmentDescriptions.size();
        vkRenderPassDesc.pAttachments = vkAttachmentDescriptions.data();
        vkRenderPassDesc.subpassCount = (uint)vkSubPasses.size();
        vkRenderPassDesc.pSubpasses = vkSubPasses.data();
        vkRenderPassDesc.dependencyCount = 0;
        vkRenderPassDesc.pDependencies = nullptr;

        VkRenderPass vkRenderPass;
        VkDevice & vkDevice = driver::Device::get()->getVulkanDevice();
        VG_ASSERT_VULKAN(vkCreateRenderPass(vkDevice, &vkRenderPassDesc, nullptr, &vkRenderPass));
        return vkRenderPass;
    }

	//--------------------------------------------------------------------------------------
	void RenderPass::finalize()
	{
        auto * device = driver::Device::get();
        VkDevice & vkDevice = device->getVulkanDevice();

        m_vkRenderPass = device->getVulkanRenderPass(m_renderPassKey);

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
        frameBufferInfo.width = device->getDeviceParams().resolution.x;
        frameBufferInfo.height = device->getDeviceParams().resolution.y;
        frameBufferInfo.layers = 1;

        VG_ASSERT_VULKAN(vkCreateFramebuffer(vkDevice, &frameBufferInfo, nullptr, &m_vkFrameBuffer));

        VkRenderPassBeginInfo & vkRenderPassBeginInfo = m_vkRenderPassBeginInfo;

        const auto & desc = m_subPasses[0]->getUserPasses()[0]->getRenderTargets()[0]->getTextureDesc();
        const auto & clearColor = desc.clearColor;
  
        vkRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        vkRenderPassBeginInfo.pNext = nullptr;
        vkRenderPassBeginInfo.renderPass = m_vkRenderPass;
        vkRenderPassBeginInfo.framebuffer = m_vkFrameBuffer;
        vkRenderPassBeginInfo.renderArea.offset.x = 0;
        vkRenderPassBeginInfo.renderArea.offset.y = 0;
        vkRenderPassBeginInfo.renderArea.extent.width = desc.width;
        vkRenderPassBeginInfo.renderArea.extent.height = desc.height;
        vkRenderPassBeginInfo.clearValueCount = 1;
        vkRenderPassBeginInfo.pClearValues = (const VkClearValue*)&clearColor;
	}
}