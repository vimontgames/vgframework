namespace vg::gfx::vulkan
{
	//--------------------------------------------------------------------------------------
	CommandList::CommandList(gfx::CommandListType _type, gfx::CommandPool * _cmdPool, core::uint _frame, core::uint _index) :
		super::CommandList(_type, _cmdPool, _frame, _index)
	{
		Device * device = gfx::Device::get();

		VkCommandBufferAllocateInfo cmdBufAllocInfo;
		cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocInfo.pNext = nullptr;
		cmdBufAllocInfo.commandPool = _cmdPool->getVulkanCommandPool();
		cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufAllocInfo.commandBufferCount = 1;

		VG_ASSERT_VULKAN(vkAllocateCommandBuffers(device->getVulkanDevice(), &cmdBufAllocInfo, &m_vkCommandBuffer));
	}

	//--------------------------------------------------------------------------------------
	CommandList::~CommandList()
	{
		// Command buffers will be automatically freed when their command pool is destroyed,
		// so we don't need an explicit cleanup.
	}

	//--------------------------------------------------------------------------------------
	void CommandList::reset()
	{
		VkCommandBufferBeginInfo cmdBufBegInfo;
		cmdBufBegInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufBegInfo.pNext = nullptr;
		cmdBufBegInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		cmdBufBegInfo.pInheritanceInfo = nullptr;

		VG_ASSERT_VULKAN(vkBeginCommandBuffer(m_vkCommandBuffer, &cmdBufBegInfo));
	}

	//--------------------------------------------------------------------------------------
	void CommandList::close()
	{
		VG_ASSERT_VULKAN(vkEndCommandBuffer(m_vkCommandBuffer));
	}

	//--------------------------------------------------------------------------------------
	void CommandList::beginRenderPass(gfx::RenderPass * _renderPass)
	{
		super::beginRenderPass(_renderPass);
        _renderPass->begin(this);	
	}

	//--------------------------------------------------------------------------------------
	void CommandList::endRenderPass()
	{
        m_renderPass->end(this); 
		super::endRenderPass();
	}

	//--------------------------------------------------------------------------------------
	void CommandList::beginSubPass(core::uint _subPassIndex, gfx::SubPass * _subPass)
	{
        super::beginSubPass(_subPassIndex, _subPass);

        // TODO
	}

	//--------------------------------------------------------------------------------------
	void CommandList::endSubPass()
	{

	}

    //--------------------------------------------------------------------------------------
    void CommandList::bindRootSignature(gfx::RootSignature * _rootSig)
    {
        auto * device = gfx::Device::get();
        auto * sig = device->getRootSignature(m_stateCache.graphicPipelineKey.m_rootSignature);

        if (sig->getVulkanDescriptorSetLayouts().size() > 0)
        {
            vector<VkDescriptorSet> vkDescriptorSets;

            vkDescriptorSets.push_back(device->m_vkBindlessDescriptors);
            vkDescriptorSets.push_back(device->m_vkSamplerDescriptors);

            vkCmdBindDescriptorSets(m_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sig->getVulkanPipelineLayout(), 0, (uint)vkDescriptorSets.size(), vkDescriptorSets.data(), 0, nullptr);
        }
    }
    
    //--------------------------------------------------------------------------------------
    void CommandList::bindGraphicPipelineState(gfx::GraphicPipelineState * _pso)
    {
        vkCmdBindPipeline(m_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pso->getVulkanPipeline());
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindPrimitiveTopology(PrimitiveTopology _topology)
    {
        /*ddd*/
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindViewport(const core::uint4 & _viewport)
    {
        VkViewport vp;
        vp.x = (float)_viewport.x;
        vp.y = (float)_viewport.y + _viewport.w;
        vp.width = (float)_viewport.z;
        vp.height = -(float)_viewport.w;
        vp.minDepth = 0.0f;
        vp.maxDepth = 1.0f;

        vkCmdSetViewport(m_vkCommandBuffer, 0, 1, &vp);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindScissor(const core::uint4 & _scissor)
    {
        VkRect2D scissor;
        scissor.offset.x = _scissor.x;
        scissor.offset.y = _scissor.y;
        scissor.extent.width = _scissor.z;
        scissor.extent.height = _scissor.w;

        vkCmdSetScissor(m_vkCommandBuffer, 0, 1, &scissor);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindRootConstants(core::uint(&_constants)[max_root_constants])
    {
        const auto vkPipelineLayout = m_currentRootSignature->getVulkanPipelineLayout();
        const auto & rootConstantDesc = m_currentRootSignature->getRootSignatureDesc().getRootConstants();
        for (uint i = 0; i < rootConstantDesc.size(); ++i)
        {
            const RootSignatureDesc::PushConstantParams & param = rootConstantDesc[i];
            vkCmdPushConstants(m_vkCommandBuffer, vkPipelineLayout, RootSignature::getVulkanShaderStageFlags(param.m_stages), param.m_start<<2, param.m_count<<2, _constants);
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindIndexBuffer(gfx::Buffer * _ib)
    {
        if (_ib)
        {
            const BufferDesc & ibDesc = _ib->getBufDesc();
            vkCmdBindIndexBuffer(m_vkCommandBuffer, _ib->getResource().getVulkanBuffer(), 0, ibDesc.elementSize == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::clear(const core::float4 & _color)
    {

    }

    //--------------------------------------------------------------------------------------
    void CommandList::draw(core::uint _vertexCount, core::uint _startOffset)
    {
        vkCmdDraw(m_vkCommandBuffer, _vertexCount, 1, _startOffset, 0);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::drawIndexed(core::uint _indexCount, core::uint _startIndex, core::uint _baseVertex)
    {
        // test
        vkCmdSetStencilReference(m_vkCommandBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, 0);

        vkCmdDrawIndexed(m_vkCommandBuffer, _indexCount, 1, _startIndex, 0, 0);
    }

    //--------------------------------------------------------------------------------------
    //void * CommandList::map(gfx::Buffer * _buffer)
    //{
    //    auto * device = gfx::Device::get();
    //    BindlessTable * bindlessTable = device->getBindlessTable();
    //    auto & context = device->getCurrentFrameContext();
    //
    //    size_t bufferSize = _buffer->getBufDesc().size();
    //
    //    //VkMemoryRequirements mem_reqs;
    //    //vkGetBufferMemoryRequirements(device->getVulkanDevice(), _buffer->getResource().getVulkanBuffer(), &mem_reqs);
    //
    //    // allocate space in upload buffer
    //    const uint alignment = 256; // CB size is required to be 256-byte aligned.
    //    const uint_ptr offset = context.m_uploadBuffer->alloc(bufferSize, alignment);
    //    core::u8 * data = context.m_uploadBuffer->getBaseAddress() + offset;
    //
    //    BindlessHandle handle = bindlessTable->allocBindlessConstantBufferHandle(context.m_uploadBuffer->getBuffer());
    //
    //    VkDescriptorBufferInfo vkBufferInfo = {};
    //    vkBufferInfo.buffer = context.m_uploadBuffer->getBuffer()->getResource().getVulkanBuffer();
    //    vkBufferInfo.offset = offset;
    //    vkBufferInfo.range = bufferSize;
    //
    //    VkBufferViewCreateInfo vkBufferViewDesc = {};
    //                           vkBufferViewDesc.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    //                           vkBufferViewDesc.buffer = context.m_uploadBuffer->getBuffer()->getResource().getVulkanBuffer();
    //                           vkBufferViewDesc.format = VK_FORMAT_R32_UINT;
    //                           vkBufferViewDesc.range = bufferSize;
    //
    //    VkBufferView  * vkBufferView = new VkBufferView();
    //    VG_ASSERT_VULKAN(vkCreateBufferView(device->getVulkanDevice(), &vkBufferViewDesc, nullptr, &vkBufferView));
    //
    //    VkWriteDescriptorSet writes = {};
    //                         writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //                         writes.dstBinding = bindless_constantbuffer_binding;
    //                         writes.descriptorCount = 1;
    //                         writes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    //                         writes.pBufferInfo = &vkBufferInfo;
    //                         writes.pTexelBufferView = &vkBufferView;;
    //                         writes.dstSet = device->getVulkanBindlessDescriptors();
    //                         writes.dstArrayElement = handle - bindless_constantbuffer_offset;
    //
    //    vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writes, 0, nullptr);
    //
    //    return data;
    //}

    //--------------------------------------------------------------------------------------
    //void CommandList::unmap(gfx::Buffer * _buffer)
    //{
    //
    //}
      
    //--------------------------------------------------------------------------------------
    void CommandList::copyBuffer(gfx::Buffer * _dst, core::uint_ptr _from)
    {
        auto * device = gfx::Device::get();
        auto & context = device->getCurrentFrameContext();

        const auto & desc = _dst->getBufDesc();

        VkBufferCopy vkBufferCopy = {};
        vkBufferCopy.srcOffset = _from; 
        vkBufferCopy.dstOffset = 0; 
        vkBufferCopy.size = desc.size();

        VkBufferMemoryBarrier vkBufMemBarrierBefore = {};
        vkBufMemBarrierBefore.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        vkBufMemBarrierBefore.pNext = NULL;
        vkBufMemBarrierBefore.srcAccessMask = 0;
        vkBufMemBarrierBefore.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkBufMemBarrierBefore.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBufMemBarrierBefore.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBufMemBarrierBefore.buffer = _dst->getResource().getVulkanBuffer();
        // Make sure anything that was copying from this image has completed 
        vkBufMemBarrierBefore.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkBufMemBarrierBefore.size = desc.size();

        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &vkBufMemBarrierBefore, 0, nullptr);

        vkCmdCopyBuffer(m_vkCommandBuffer, device->getUploadBuffer()->getBuffer()->getResource().getVulkanBuffer(), _dst->getResource().getVulkanBuffer(), 1, &vkBufferCopy);

        VkBufferMemoryBarrier vkBufMemBarrierAfter = {};
        vkBufMemBarrierAfter.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        vkBufMemBarrierAfter.pNext = NULL;
        vkBufMemBarrierAfter.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkBufMemBarrierAfter.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        vkBufMemBarrierAfter.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBufMemBarrierAfter.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBufMemBarrierAfter.buffer = _dst->getResource().getVulkanBuffer();
        vkBufMemBarrierAfter.size = desc.size();
        
        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 1, &vkBufMemBarrierAfter, 0, nullptr);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::copyTexture(gfx::Texture * _dst, core::uint_ptr _from)
    {
        auto * device = gfx::Device::get();
        auto & context = device->getCurrentFrameContext();

        const auto & desc = _dst->getTexDesc();

        VkImageMemoryBarrier image_memory_barrier = {};
                             image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                             image_memory_barrier.pNext = NULL;
                             image_memory_barrier.srcAccessMask = 0;
                             image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                             image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                             image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                             image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
                             image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                             image_memory_barrier.image = _dst->getResource().getVulkanImage();
                             image_memory_barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, desc.mipmaps, 0, 1 };
                             // Make sure anything that was copying from this image has completed 
                             image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

        VkDeviceSize currentOffset = _from;

        for (uint i = 0; i < desc.mipmaps; ++i)
        {
            const uint w = desc.width >> i;
            const uint h = desc.height >> i;

            VkBufferImageCopy vkBufImgCopy = {};
                              vkBufImgCopy.bufferOffset = currentOffset;
                              vkBufImgCopy.bufferRowLength = 0;
                              vkBufImgCopy.bufferImageHeight = 0;
                              vkBufImgCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                              vkBufImgCopy.imageSubresource.mipLevel = i;
                              vkBufImgCopy.imageSubresource.baseArrayLayer = 0;
                              vkBufImgCopy.imageSubresource.layerCount = 1;
                              vkBufImgCopy.imageOffset = { 0, 0, 0 };
                              vkBufImgCopy.imageExtent = { w, h, 1 };

            vkCmdCopyBufferToImage(m_vkCommandBuffer, device->getUploadBuffer()->getBuffer()->getResource().getVulkanBuffer(), _dst->getResource().getVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufImgCopy);

            currentOffset += w * h * Texture::getPixelFormatSize(desc.format);
        }

        VkImageMemoryBarrier image_memory_barrier2 = {};
        image_memory_barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barrier2.pNext = NULL;
        image_memory_barrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        image_memory_barrier2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        image_memory_barrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        image_memory_barrier2.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        image_memory_barrier2.image = _dst->getResource().getVulkanImage();
        image_memory_barrier2.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, desc.mipmaps, 0, 1};

        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier2);
    }

    //--------------------------------------------------------------------------------------
    float4 asFloat4(u32 _color)
    {
        float4 color = float4((_color & 0xFF), (_color >> 8) & 0xFF, (_color >> 16) & 0xFF, (_color >> 24) & 0xFF);
        return color / float4(255.0f);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::beginGPUEvent(const char * _name, core::u32 _color)
    {
        #ifdef VG_ENABLE_GPU_MARKER 
        VkDebugUtilsLabelEXT markerInfo = {};
        markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        float4 color = asFloat4(_color);
        markerInfo.color[0] = color.r;
        markerInfo.color[1] = color.g;
        markerInfo.color[2] = color.b;
        markerInfo.color[3] = color.a;
        markerInfo.pLabelName = _name;

        auto * device = gfx::Device::get();
        device->m_EXT_DebugUtils.m_pfnCmdBeginDebugUtilsLabelEXT(getVulkanCommandBuffer(), &markerInfo);
        #endif
    }

    //--------------------------------------------------------------------------------------
    void CommandList::endGPUEvent()
    {
        #ifdef VG_ENABLE_GPU_MARKER 
        auto * device = gfx::Device::get();
        device->m_EXT_DebugUtils.m_pfnCmdEndDebugUtilsLabelEXT(getVulkanCommandBuffer());
        #endif
    }
}