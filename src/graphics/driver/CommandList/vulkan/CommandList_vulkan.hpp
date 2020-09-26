namespace vg::graphics::driver::vulkan
{
	//--------------------------------------------------------------------------------------
	CommandList::CommandList(driver::CommandListType _type, driver::CommandPool * _cmdPool, core::uint _frame, core::uint _index) :
		super::CommandList(_type, _cmdPool, _frame, _index)
	{
		Device * device = driver::Device::get();

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
		cmdBufBegInfo.flags = 0;
		cmdBufBegInfo.pInheritanceInfo = nullptr;

		VG_ASSERT_VULKAN(vkBeginCommandBuffer(m_vkCommandBuffer, &cmdBufBegInfo));
	}

	//--------------------------------------------------------------------------------------
	void CommandList::close()
	{
		VG_ASSERT_VULKAN(vkEndCommandBuffer(m_vkCommandBuffer));
	}

	//--------------------------------------------------------------------------------------
	void CommandList::beginRenderPass(const driver::RenderPass * _renderPass)
	{
		super::beginRenderPass(_renderPass);

		vkCmdBeginRenderPass(m_vkCommandBuffer, &_renderPass->getVulkanRenderPassBeginInfo(), VK_SUBPASS_CONTENTS_INLINE);
	}

	//--------------------------------------------------------------------------------------
	void CommandList::endRenderPass()
	{
		vkCmdEndRenderPass(m_vkCommandBuffer);

		super::endRenderPass();
	}

	//--------------------------------------------------------------------------------------
	void CommandList::beginSubPass(core::uint _subPassIndex, const driver::SubPass * _subPass)
	{
        super::beginSubPass(_subPassIndex, _subPass);

        // TODO
	}

	//--------------------------------------------------------------------------------------
	void CommandList::endSubPass()
	{

	}

    //--------------------------------------------------------------------------------------
    void CommandList::bindRootSignature(driver::RootSignature * _rootSig)
    {
        auto * device = driver::Device::get();
        auto * sig = device->getRootSignature(m_stateCache.graphicPipelineKey.m_rootSignature);

        if (sig->getVulkanDescriptorSetLayouts().size() > 0)
        {
            vkCmdBindDescriptorSets(m_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sig->getVulkanPipelineLayout(), 0, 2, device->m_vkbindlessDescriptorSet, 0, nullptr);
        }
    }
    
    //--------------------------------------------------------------------------------------
    void CommandList::bindGraphicPipelineState(driver::GraphicPipelineState * _pso)
    {
        vkCmdBindPipeline(m_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pso->getVulkanPipeline());
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindPrimitiveTopology(PrimitiveTopology _topology)
    {

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
    void CommandList::clear(const core::float4 & _color)
    {

    }

    //--------------------------------------------------------------------------------------
    void CommandList::draw(core::uint _vertexCount, core::uint _startOffset)
    {
        vkCmdDraw(m_vkCommandBuffer, _vertexCount, 1, _startOffset, 0);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::copyTexture(driver::Texture * _dst, core::u32 _from)
    {
        auto * device = driver::Device::get();
        auto & context = device->getCurrentFrameContext();

        const auto & desc = _dst->getTexDesc();

        VkBufferImageCopy vkBufImgCopy = {};
        vkBufImgCopy.bufferOffset = _from;
        vkBufImgCopy.bufferRowLength = 0;
        vkBufImgCopy.bufferImageHeight = 0;
        vkBufImgCopy.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        vkBufImgCopy.imageOffset = { 0, 0, 0 };
        vkBufImgCopy.imageExtent = { desc.width, desc.height, 1 };

            //   demo_set_image_layout(demo, demo->textures[i].image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED,
            //       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            //       VK_PIPELINE_STAGE_TRANSFER_BIT);

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
        image_memory_barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        /* Make sure anything that was copying from this image has completed */
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);

        vkCmdCopyBufferToImage(m_vkCommandBuffer, context.m_uploadBuffer->getResource().getVulkanBuffer() , _dst->getResource().getVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufImgCopy);

        // demo_set_image_layout(demo, demo->textures[i].image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        //demo->textures[i].imageLayout, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        //    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

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
        image_memory_barrier2.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier2);
    }
}