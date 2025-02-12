namespace vg::gfx::vulkan
{
	//--------------------------------------------------------------------------------------
	RenderPass::RenderPass(RenderPassType _renderPassType, const RenderPassKey & _key) :
        super::RenderPass(_renderPassType, _key)
	{

	}

	//--------------------------------------------------------------------------------------
	RenderPass::~RenderPass()
	{
        gfx::Device * device = gfx::Device::get();
        VkDevice & vkDevice = device->getVulkanDevice();
        
        //vkDestroyRenderPass(vkDevice, m_vkRenderPass, nullptr);
        device->releaseVulkanRenderPass(getRenderPassKey(), m_vkRenderPass);

        vkDestroyFramebuffer(vkDevice, m_vkFrameBuffer, nullptr);
	}

    //--------------------------------------------------------------------------------------
    bool RenderPass::createVulkanAttachmentDesc(PixelFormat _format, MSAA _msaa, const ResourceTransitionDesc & _info, VkAttachmentDescription * _att)
    {
        if (PixelFormat::Unknow == _format)
            return false;

        const bool isDepthStencil = Texture::isDepthStencilFormat(_format);

        const auto optimalInitialLayout = isDepthStencil ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        _att->flags = (VkAttachmentDescriptionFlags)0;
        _att->format = Texture::getVulkanPixelFormat(_format);

        // TODO: RenderPassKey flags for initial/final state for each attachment
        _att->samples = (VkSampleCountFlagBits)TextureDesc::getMSAASampleCount(_msaa);

        if (asBool(ResourceTransitionFlags::Clear & _info.flags))
            _att->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        else if (asBool(ResourceTransitionFlags::Preserve & _info.flags))
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
                _att->initialLayout = isDepthStencil ? VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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
                _att->finalLayout = isDepthStencil ? VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;// ; // TODO: Read-only depthstencil using VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
                break;
        }

        if (asBool(_info.flags & ResourceTransitionFlags::Present))
            _att->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        if (isDepthStencil)
        {
            _att->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            if (asBool(ResourceTransitionFlags::Clear & _info.flags))
            {
                _att->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                _att->stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            }
            else if (asBool(ResourceTransitionFlags::Preserve & _info.flags))
            {
                _att->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                _att->stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            }   
            else
            {
                _att->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                _att->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            }
        }
        else
        {
            _att->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            _att->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            _att->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }       

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
            const ResourceTransitionDesc & info = subPassKey.getColorAttachmentInfo(i);
            const PixelFormat format = _key.m_colorFormat[i];
            
            VkAttachmentDescription att;

            if (createVulkanAttachmentDesc(format, _key.m_msaa, info, &att))
                vkAttachmentDescriptions.push_back(att);
        }

        // depthstencil
        const ResourceTransitionDesc & info = subPassKey.getDepthStencilAttachmentInfo();
        const PixelFormat format = _key.m_depthStencilFormat;

        VkAttachmentDescription att;

        if (createVulkanAttachmentDesc(format, _key.m_msaa, info, &att))
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
                const ResourceTransitionDesc & info = subPassKey.getColorAttachmentInfo(j);

                if (asBool(ResourceTransitionFlags::RenderTarget & info.flags))
                {
                    const PixelFormat format = _key.m_colorFormat[i];

                    if (Texture::isDepthStencilFormat(format))
                        continue;

                    VkAttachmentReference colorAttRef = { j, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
                    vkSubPassAttachmentRefs.push_back(colorAttRef);

                    ++renderTargetCount;
                }
            }

            const ResourceTransitionDesc & info = subPassKey.getDepthStencilAttachmentInfo();
            if (asBool(ResourceTransitionFlags::RenderTarget & info.flags))
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

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo vkRenderPassDesc;
        vkRenderPassDesc.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        vkRenderPassDesc.pNext = nullptr;
        vkRenderPassDesc.flags = 0;
        vkRenderPassDesc.attachmentCount = (uint)vkAttachmentDescriptions.size();
        vkRenderPassDesc.pAttachments = vkAttachmentDescriptions.data();
        vkRenderPassDesc.subpassCount = (uint)vkSubPasses.size();
        vkRenderPassDesc.pSubpasses = vkSubPasses.data();
        vkRenderPassDesc.dependencyCount = 1;
        vkRenderPassDesc.pDependencies = &dependency;

        VkRenderPass vkRenderPass;
        VkDevice & vkDevice = gfx::Device::get()->getVulkanDevice();
        VG_VERIFY_VULKAN(vkCreateRenderPass(vkDevice, &vkRenderPassDesc, nullptr, &vkRenderPass)); 
        return vkRenderPass;
    }

	//--------------------------------------------------------------------------------------
	void RenderPass::finalize()
	{
        auto * device = gfx::Device::get();
        VkDevice & vkDevice = device->getVulkanDevice();

        const auto * userPass = m_subPasses[0]->getUserPassesInfos()[0].m_userPass;
        const auto & renderTargets = userPass->getRenderTargets();
        
        m_vkClearValues.reserve(renderTargets.size() + 1);

        u32 width = -1; 
        u32 height = -1;

        for (uint i = 0; i < renderTargets.size(); ++i)
        {
            const auto & desc = renderTargets[i]->getTextureResourceDesc();

            if (-1 != width || -1 != height)
                VG_ASSERT(width == desc.width || height == desc.height, "RenderTarget[%u] has a size of %ux%u instead of %ux%u. All bound RenderTargets must have the same dimensions", i, desc.width, desc.height, width, height);

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

            if (-1 != width || -1 != height)
                VG_ASSERT(width == desc.width || height == desc.height, "DepthStencil has a size of %ux%u instead of %ux%u. DepthStencil must have the same dimensions as bound RenderTargets", desc.width, desc.height, width, height);

            width = desc.width;
            height = desc.height;

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
        auto * device = gfx::Device::get();
        VkDevice & vkDevice = device->getVulkanDevice();

        if (m_colorAttachments.size() > 0 || nullptr != m_depthStencilAttachment)
        {
            core::vector<VkImageView> vkImageViews;

            for (const FrameGraphTextureResource * res : m_colorAttachments)
                vkImageViews.push_back(res->getTexture()->getVulkanImageView());

            if (m_depthStencilAttachment)
                vkImageViews.push_back(m_depthStencilAttachment->getTexture()->getVulkanImageView());

            m_vkFrameBufferInfo.attachmentCount = (uint)vkImageViews.size();
            m_vkFrameBufferInfo.pAttachments = vkImageViews.data();
            m_vkFrameBufferInfo.layers = 1;

            VG_VERIFY_VULKAN(vkCreateFramebuffer(vkDevice, &m_vkFrameBufferInfo, nullptr, &m_vkFrameBuffer));

            m_vkRenderPassBeginInfo.framebuffer = m_vkFrameBuffer;

            vkCmdBeginRenderPass(_cmdList->getVulkanCommandBuffer(), &m_vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        }
    }

    //--------------------------------------------------------------------------------------
    void RenderPass::end(CommandList * _cmdList)
    {
        if (m_colorAttachments.size() > 0 || nullptr != m_depthStencilAttachment)
        {
            vkCmdEndRenderPass(_cmdList->getVulkanCommandBuffer());

            //auto * device = gfx::Device::get();
            //VkDevice & vkDevice = device->getVulkanDevice();
            //vkDestroyFramebuffer(vkDevice, m_vkFrameBuffer, nullptr);
        }
    }
}