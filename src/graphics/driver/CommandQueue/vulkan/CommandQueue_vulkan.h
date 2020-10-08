#pragma once

#include "graphics/driver/CommandQueue/CommandQueue.h"

namespace vg::graphics::driver::vulkan
{
	class CommandQueue : public base::CommandQueue
	{
		using super = base::CommandQueue;

	public:
		CommandQueue(CommandQueueType _type);
		~CommandQueue();

        VkQueue & getVulkanCommandQueue() { return m_vkCommandQueue; }

	//private:
		VkQueue	m_vkCommandQueue;
	};
}