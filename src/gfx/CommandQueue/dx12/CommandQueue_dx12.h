#pragma once

#include "gfx/CommandQueue/CommandQueue.h"

namespace vg::gfx::dx12
{
	class CommandQueue : public base::CommandQueue
	{
		using super = base::CommandQueue;

	public:
		CommandQueue(CommandQueueType _type);
		~CommandQueue();

		ID3D12CommandQueue * getd3d12CommandQueue() const { return m_d3d12commandQueue; }

	private:
		ID3D12CommandQueue * m_d3d12commandQueue = nullptr;
	};
}