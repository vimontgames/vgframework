#pragma once

#include "graphics/driver/CommandQueue/CommandQueue.h"

namespace vg::graphics::driver::vulkan
{
	class CommandQueue : public base::CommandQueue
	{
		using super = base::CommandQueue;

	public:
		CommandQueue(CommandQueueType _type, core::uint _index);
		~CommandQueue();

	//private:
		VkQueue	m_vkCommandQueue;
	};
}