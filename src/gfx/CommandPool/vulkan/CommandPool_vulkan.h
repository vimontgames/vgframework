#pragma once

#include "gfx/CommandPool/CommandPool.h"

namespace vg::gfx::vulkan
{
	class CommandPool : public base::CommandPool
	{
		using super = base::CommandPool;

	public:
		CommandPool(core::uint _frame, core::uint _index);
		~CommandPool();

		void beginFrame();

		VkCommandPool & getVulkanCommandPool() { return m_vulkanCmdPool; }

	private:
		VkCommandPool m_vulkanCmdPool = VK_NULL_HANDLE;
	};
}