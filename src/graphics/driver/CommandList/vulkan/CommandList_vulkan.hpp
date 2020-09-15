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

        // test
        VkDescriptorSetAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.pNext = nullptr;
        alloc_info.descriptorPool = device->m_vkDescriptorPool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &sig->getVulkanDescriptorSetLayouts()[0];

        static VkDescriptorSet vkDescriptorSet;
        static bool done = false;

        if (!done)
        {
            vkAllocateDescriptorSets(device->getVulkanDevice(), &alloc_info, &vkDescriptorSet);
            done = true;
        }

        vkCmdBindDescriptorSets(getVulkanCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, sig->getVulkanPipelineLayout(), 0, 1, &vkDescriptorSet, 0, nullptr);
    }
    
    //--------------------------------------------------------------------------------------
    void CommandList::bindGraphicPipelineState(driver::GraphicPipelineState * _pso)
    {
        vkCmdBindPipeline(getVulkanCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pso->getVulkanPipeline());
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

        vkCmdSetViewport(getVulkanCommandBuffer(), 0, 1, &vp);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindScissor(const core::uint4 & _scissor)
    {
        VkRect2D scissor;
        scissor.offset.x = _scissor.x;
        scissor.offset.y = _scissor.y;
        scissor.extent.width = _scissor.z;
        scissor.extent.height = _scissor.w;

        vkCmdSetScissor(getVulkanCommandBuffer(), 0, 1, &scissor);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::clear(const core::float4 & _color)
    {

    }

    //--------------------------------------------------------------------------------------
    void CommandList::draw(core::uint _vertexCount, core::uint _startOffset)
    {
        vkCmdDraw(getVulkanCommandBuffer(), _vertexCount, 1, _startOffset, 0);
    }
}