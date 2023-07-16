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
    bool RenderPass::createVulkanAttachmentDesc(PixelFormat _format, const SubPassKey::AttachmentInfo & _info, VkAttachmentDescription * _att)
    {
        if (PixelFormat::Unknow == _format)
            return false;

        const bool isDepthStencil = Texture::isDepthStencilFormat(_format);

        const auto optimalInitialLayout = isDepthStencil ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        _att->flags = (VkAttachmentDescriptionFlags)0;
        _att->format = Texture::getVulkanPixelFormat(_format);

        // TODO: RenderPassKey flags for initial/final state for each attachment
        _att->samples = VK_SAMPLE_COUNT_1_BIT;

        if (asBool(SubPassKey::AttachmentFlags::Clear & _info.flags))
            _att->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        else if (asBool(SubPassKey::AttachmentFlags::Preserve & _info.flags))
            _att->loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        else
            _att->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

        switch (_info.begin)
        {
            case ResourceState::Undefined:
                _att->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                break;

            case ResourceState::RenderTarget:
                _att->initialLayout = optimalInitialLayout;
                break;

            case ResourceState::ShaderResource:
                _att->initialLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
                break;
        }

        switch (_info.end)
        {
            case ResourceState:: Undefined:
                _att->finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                break;

            case ResourceState::RenderTarget:
                _att->finalLayout = optimalInitialLayout;
                break;

            case ResourceState::ShaderResource:
                _att->finalLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
                break;
        }

        if (asBool(_info.flags & SubPassKey::AttachmentFlags::Present))
            _att->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        _att->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        _att->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        _att->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        return true;
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
            const PixelFormat format = _key.m_colorFormat[i];
            
            VkAttachmentDescription att;

            if (createVulkanAttachmentDesc(format, info, &att))
                vkAttachmentDescriptions.push_back(att);
        }

        // depthstencil
        const SubPassKey::AttachmentInfo & info = subPassKey.getDepthStencilAttachmentInfo();
        const PixelFormat format = _key.m_depthStencilFormat;

        VkAttachmentDescription att;

        if (createVulkanAttachmentDesc(format, info, &att))
            vkAttachmentDescriptions.push_back(att);

        // Add subpass descriptions
        vector<VkSubpassDescription> vkSubPasses;

        // Foreach subpass we need to say the attachment used
        vector<vector<VkAttachmentReference>> vkAttachmentRefs;
        vector<VkAttachmentReference> vkDepthStencilAttachmentRefs; // should use only a pointer ?

        // No realloc please
        vkAttachmentRefs.resize(_key.m_subPassCount);

        for (uint i = 0; i < _key.m_subPassCount; ++i)
        {
            const auto subPassKey = _key.m_subPassKeys[i];

            VkSubpassDescription vkSubPass = {};
            vector<VkAttachmentReference> & vkSubPassAttachmentRefs = vkAttachmentRefs[i];

            uint renderTargetCount = 0;
            
            for (uint j = 0; j < maxRenderTarget; ++j) // maxRenderTarget == maxAttachment per subPass ?
            {
                const SubPassKey::AttachmentInfo & info = subPassKey.getColorAttachmentInfo(j);

                if (asBool(SubPassKey::AttachmentFlags::RenderTarget & info.flags))
                {
                    const PixelFormat format = _key.m_colorFormat[i];

                    if (Texture::isDepthStencilFormat(format))
                        continue;

                    VkAttachmentReference colorAttRef = { i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
                    vkSubPassAttachmentRefs.push_back(colorAttRef);

                    ++renderTargetCount;
                }
            }

            const SubPassKey::AttachmentInfo & info = subPassKey.getDepthStencilAttachmentInfo();
            if (asBool(SubPassKey::AttachmentFlags::RenderTarget & info.flags))
            {
                const PixelFormat format = _key.m_depthStencilFormat;

                if (Texture::isDepthStencilFormat(format))
                {
                    VkAttachmentReference depthStencilAttRef = { renderTargetCount, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
                    vkDepthStencilAttachmentRefs.push_back(depthStencilAttRef);
                }
            }

            vkSubPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            vkSubPass.flags = 0;
            vkSubPass.inputAttachmentCount = 0;
            vkSubPass.pInputAttachments = nullptr;
            vkSubPass.colorAttachmentCount = (uint)vkSubPassAttachmentRefs.size();
            vkSubPass.pColorAttachments = vkSubPassAttachmentRefs.data();
            vkSubPass.pResolveAttachments = nullptr;
            vkSubPass.pDepthStencilAttachment = vkDepthStencilAttachmentRefs.size() > 0 ? &vkDepthStencilAttachmentRefs[0] : nullptr;
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

        const auto * userPass = m_subPasses[0]->getUserPassesInfos()[0].m_userPass;
        const auto & renderTargets = userPass->getRenderTargets();
        
        m_vkClearValues.reserve(renderTargets.size() + 1);

        u32 width = 0; 
        u32 height = 0;

        for (uint i = 0; i < renderTargets.size(); ++i)
        {
            const auto & desc = renderTargets[i]->getTextureResourceDesc();

            VG_ASSERT(width == 0 || width == desc.width);
            VG_ASSERT(height == 0 || height == desc.height);

            width = desc.width;
            height = desc.height;

            VkClearValue vkClearColorValue;
                         vkClearColorValue.color.float32[0] = desc.clearColor.x;
                         vkClearColorValue.color.float32[1] = desc.clearColor.y;
                         vkClearColorValue.color.float32[2] = desc.clearColor.z;
                         vkClearColorValue.color.float32[3] = desc.clearColor.w;

            m_vkClearValues.push_back(vkClearColorValue);
        }

        if (userPass->hasDepthStencil())
        {
            const auto & depthStencil = userPass->getDepthStencil();
            const auto & desc = depthStencil->getTextureResourceDesc();

            VG_ASSERT(width == 0 || width == desc.width);
            VG_ASSERT(height == 0 || height == desc.height);

            VkClearValue vkDepthStencilClearValue;
                         vkDepthStencilClearValue.depthStencil.depth = desc.clearDepth;
                         vkDepthStencilClearValue.depthStencil.stencil = desc.clearStencil;

            m_vkClearValues.push_back(vkDepthStencilClearValue);
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
        for (const FrameGraph::TextureResource * res : m_colorAttachments)
            vkImageViews.push_back(res->getTexture()->getVulkanImageView());

        if (m_depthStencilAttachment)
            vkImageViews.push_back(m_depthStencilAttachment->getTexture()->getVulkanImageView());

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