namespace vg::gfx::vulkan
{
	//--------------------------------------------------------------------------------------
	CommandQueue::CommandQueue(CommandQueueType _type) :
		base::CommandQueue(_type)
	{
		auto * device = gfx::Device::get();
		const auto familyIndex = device->getVulkanCommandQueueFamilyIndex(_type);
		vkGetDeviceQueue(device->getVulkanDevice(), familyIndex, 0, &m_vkCommandQueue);
	}

	//--------------------------------------------------------------------------------------
	CommandQueue::~CommandQueue()
	{

	}
}