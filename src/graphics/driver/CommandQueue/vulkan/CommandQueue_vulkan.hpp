namespace vg::graphics::driver::vulkan
{
	//--------------------------------------------------------------------------------------
	CommandQueue::CommandQueue(CommandQueueType _type, core::uint _index) :
		base::CommandQueue(_type, _index)
	{
		auto * device = driver::Device::get();
		const auto familyIndex = device->getVulkanCommandQueueFamilyIndex(_type);
		vkGetDeviceQueue(device->getVulkanDevice(), familyIndex, _index, &m_vkCommandQueue);
	}

	//--------------------------------------------------------------------------------------
	CommandQueue::~CommandQueue()
	{

	}
}