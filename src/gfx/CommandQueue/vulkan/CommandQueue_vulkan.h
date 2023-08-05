#pragma once

#include "gfx/CommandQueue/CommandQueue.h"

namespace vg::gfx::vulkan
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