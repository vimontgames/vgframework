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

		vkCmdBeginRenderPass(m_vkCommandBuffer, &_renderPass->m_vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
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
		
	}

	//--------------------------------------------------------------------------------------
	void CommandList::endSubPass()
	{

	}
}