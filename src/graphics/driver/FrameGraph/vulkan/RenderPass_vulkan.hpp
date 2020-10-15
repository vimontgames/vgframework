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

        VG_ASSERT(1 == _key.m_subPassCount);
        const auto subPassKey = _key.m_subPassKeys[0];

        for (uint i = 0; i < maxRenderTarget; ++i)
        {
            const SubPassKey::AttachmentInfo & info = subPassKey.getColorAttachmentInfo(i);

            const auto format = _key.m_colorFormat[i];
            if (PixelFormat::Unknow != format)
            {
                VkAttachmentDescription att;

                att.flags = (VkAttachmentDescriptionFlags)0;
                att.format = Texture::getVulkanPixelFormat(format);

                // TODO: RenderPassKey flags for initial/final state for each attachment
                att.samples = VK_SAMPLE_COUNT_1_BIT;

                if (asBool(SubPassKey::AttachmentFlags::Clear & info.flags))
                {
                    att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    //att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                    //att.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                }
                else if (asBool(SubPassKey::AttachmentFlags::Preserve & info.flags))
                {
                    att.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                    //att.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    //att.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                }
                else
                {
                    att.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    //att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                    //att.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                }

                switch (info.begin)
                {
                case ResourceState::Undefined:
                    att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                    break;

                case ResourceState::RenderTarget:
                    att.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    break;

                case ResourceState::ShaderResource:
                    att.initialLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
                    break;
                }

                switch (info.end)
                {
                    case ResourceState:: Undefined:
                        att.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                        break;

                    case ResourceState::RenderTarget:
                        att.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                        break;

                    case ResourceState::ShaderResource:
                        att.finalLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
                        break;
                }

                if (asBool(info.flags & SubPassKey::AttachmentFlags::Present))
                    att.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

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
                const SubPassKey::AttachmentInfo & info = subPassKey.getColorAttachmentInfo(j);

                if (asBool(SubPassKey::AttachmentFlags::RenderTarget & info.flags))
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

        const auto & renderTargets = m_subPasses[0]->getUserPasses()[0]->getRenderTargets();
        
        m_vkClearValues.resize(renderTargets.size());
        u32 width = 0; 
        u32 height = 0;

        for (uint i = 0; i < renderTargets.size(); ++i)
        {
            const auto & desc = renderTargets[i]->getTextureResourceDesc();

            VG_ASSERT(width == 0 || width == desc.width);
            VG_ASSERT(height == 0 || height == desc.height);

            width = desc.width;
            height = desc.height;

            VkClearValue vkClearValue;
            vkClearValue.color.float32[0] = desc.clearColor.x;
            vkClearValue.color.float32[1] = desc.clearColor.y;
            vkClearValue.color.float32[2] = desc.clearColor.z;
            vkClearValue.color.float32[3] = desc.clearColor.w;
            m_vkClearValues[i] = vkClearValue;
        }

        m_vkRenderPass = device->getVulkanRenderPass(m_renderPassKey);
        m_vkFrameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        m_vkFrameBufferInfo.flags = 0;
        m_vkFrameBufferInfo.pNext = nullptr;
        m_vkFrameBufferInfo.renderPass = m_vkRenderPass;

        m_vkFrameBufferInfo.width = width;
        m_vkFrameBufferInfo.height = height;
        m_vkFrameBufferInfo.layers = 1;
        
        m_vkRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        m_vkRenderPassBeginInfo.pNext = nullptr;
        m_vkRenderPassBeginInfo.renderPass = m_vkRenderPass;
        m_vkRenderPassBeginInfo.renderArea.offset.x = 0;
        m_vkRenderPassBeginInfo.renderArea.offset.y = 0;
        m_vkRenderPassBeginInfo.renderArea.extent.width = width;
        m_vkRenderPassBeginInfo.renderArea.extent.height = height;
        m_vkRenderPassBeginInfo.clearValueCount = (uint)m_vkClearValues.size();
        m_vkRenderPassBeginInfo.pClearValues = (const VkClearValue*)m_vkClearValues.data();
	}

    //--------------------------------------------------------------------------------------
    void RenderPass::begin(CommandList * _cmdList)
    {
        auto * device = driver::Device::get();
        VkDevice & vkDevice = device->getVulkanDevice();

        core::vector<VkImageView> vkImageViews;
        for (const FrameGraph::TextureResource * res : m_attachments)
            vkImageViews.push_back(res->getTexture()->getVulkanImageView());

        m_vkFrameBufferInfo.attachmentCount = (uint)vkImageViews.size();
        m_vkFrameBufferInfo.pAttachments = vkImageViews.data();

        VG_ASSERT_VULKAN(vkCreateFramebuffer(vkDevice, &m_vkFrameBufferInfo, nullptr, &m_vkFrameBuffer));

        m_vkRenderPassBeginInfo.framebuffer = m_vkFrameBuffer;

        vkCmdBeginRenderPass(_cmdList->getVulkanCommandBuffer(), &m_vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    //--------------------------------------------------------------------------------------
    void RenderPass::end(CommandList * _cmdList)
    {
        vkCmdEndRenderPass(_cmdList->getVulkanCommandBuffer());

        auto * device = driver::Device::get();
        VkDevice & vkDevice = device->getVulkanDevice();

        //vkDestroyFramebuffer(vkDevice, m_vkFrameBuffer, nullptr);
    }
}