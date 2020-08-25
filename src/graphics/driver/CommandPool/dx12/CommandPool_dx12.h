#pragma once

#include "graphics/driver/CommandPool/CommandPool.h"

namespace vg::graphics::driver::dx12
{
	class CommandPool : public base::CommandPool
	{
		using super = base::CommandPool;

	public:
		CommandPool(core::uint _frame, core::uint _index);
		~CommandPool();

		void beginFrame();

		ID3D12CommandAllocator * getd3d12CommandAllocator() { return m_d3d12cmdAllocator; }

	private:
		ID3D12CommandAllocator * m_d3d12cmdAllocator = nullptr;
	};
}