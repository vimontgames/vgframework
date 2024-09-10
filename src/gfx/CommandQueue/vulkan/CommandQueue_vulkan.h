#pragma once

#include "gfx/CommandQueue/CommandQueue.h"

namespace vg::gfx
{
	class CommandList;
}

namespace vg::gfx::vulkan
{
	class CommandQueue : public base::CommandQueue
	{
		using super = base::CommandQueue;

	public:
		CommandQueue(CommandQueueType _type);
		~CommandQueue();

        VkQueue & getVulkanCommandQueue() { return m_vkCommandQueue; }

        void beginFrame(gfx::CommandList * _cmdList);
        void endFrame(gfx::CommandList * _cmdList);

	private:
		VkQueue	m_vkCommandQueue;
		VkQueryPool m_vkQueryPool;
		core::uint m_queryIndex = 0;
		float m_timestampPeriod = 0.0f;
	};
}