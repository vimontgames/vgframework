namespace vg::gfx::vulkan
{
	//--------------------------------------------------------------------------------------
	CommandPool::CommandPool(uint _frame, uint _index) :
		super::CommandPool(_frame, _index)
	{
		auto * device = gfx::Device::get();
		
		VkCommandPoolCreateInfo cmdPoolCreateInfo;
		cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolCreateInfo.pNext = nullptr;
		cmdPoolCreateInfo.queueFamilyIndex = device->getVulkanCommandQueueFamilyIndex(CommandQueueType::Graphics);
		cmdPoolCreateInfo.flags = 0;

		VG_VERIFY_VULKAN(vkCreateCommandPool(device->getVulkanDevice(), &cmdPoolCreateInfo, nullptr, &m_vulkanCmdPool));
	}

	//--------------------------------------------------------------------------------------
	CommandPool::~CommandPool()
	{
		auto * device = gfx::Device::get();

		vkDestroyCommandPool(device->getVulkanDevice(), m_vulkanCmdPool, nullptr);
	}

	//--------------------------------------------------------------------------------------
	void CommandPool::beginFrame()
	{
        auto * device = gfx::Device::get();

        VG_VERIFY_VULKAN(vkResetCommandPool(device->getVulkanDevice(), m_vulkanCmdPool, 0));
	}
}