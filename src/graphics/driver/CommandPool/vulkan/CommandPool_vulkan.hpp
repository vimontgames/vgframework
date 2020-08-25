namespace vg::graphics::driver::vulkan
{
	//--------------------------------------------------------------------------------------
	CommandPool::CommandPool(uint _frame, uint _index) :
		super::CommandPool(_frame, _index)
	{
		auto * device = driver::Device::get();
		
		VkCommandPoolCreateInfo cmdPoolCreateInfo;
		cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolCreateInfo.pNext = nullptr;
		cmdPoolCreateInfo.queueFamilyIndex = device->getVulkanCommandQueueFamilyIndex(CommandQueueType::Graphics);
		cmdPoolCreateInfo.flags = 0;

		VG_ASSERT_VULKAN(vkCreateCommandPool(device->getVulkanDevice(), &cmdPoolCreateInfo, nullptr, &m_vulkanCmdPool));
	}

	//--------------------------------------------------------------------------------------
	CommandPool::~CommandPool()
	{
		auto * device = driver::Device::get();

		vkDestroyCommandPool(device->getVulkanDevice(), m_vulkanCmdPool, nullptr);
	}

	//--------------------------------------------------------------------------------------
	void CommandPool::beginFrame()
	{
        auto * device = driver::Device::get();

        VG_ASSERT_VULKAN(vkResetCommandPool(device->getVulkanDevice(), m_vulkanCmdPool, 0));
	}
}