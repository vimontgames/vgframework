#pragma once

#include "graphics/driver/CommandList/CommandList.h"

namespace vg::graphics::driver::vulkan
{
	class CommandList : public base::CommandList
	{
		using super = base::CommandList;

	public:
		CommandList(driver::CommandListType _type, driver::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
		~CommandList();

		void								reset();
		void								close();

		void								beginRenderPass(const driver::RenderPass * _renderPass);
		void								endRenderPass();

		void								beginSubPass(core::uint _subPassIndex, const driver::SubPass * _subPass);
		void								endSubPass();

		VkCommandBuffer &					getVulkanCommandBuffer() { return m_vkCommandBuffer;  }

	private:
		VkCommandBuffer						m_vkCommandBuffer;
	};
}