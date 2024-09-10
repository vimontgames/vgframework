#pragma once

#include "gfx/CommandQueue/CommandQueue.h"

namespace vg::gfx
{
	class CommandList;
	class Buffer;
}

namespace vg::gfx::dx12
{
	class CommandQueue : public base::CommandQueue
	{
		using super = base::CommandQueue;

	public:
		CommandQueue(CommandQueueType _type);
		~CommandQueue();

		ID3D12CommandQueue * getd3d12CommandQueue() const { return m_d3d12commandQueue; }

		void beginFrame(gfx::CommandList * _cmdList);
		void endFrame(gfx::CommandList * _cmdList);

		core::i64 GetCPUTimestamp(core::i64 gpuTimestamp);

	private:
		ID3D12CommandQueue * m_d3d12commandQueue = nullptr;

		ID3D12QueryHeap * m_d3d12queryHeap = nullptr;
        core::u64 m_frequencyCPU = 0;
        core::u64 m_frequencyGPU = 0;
        core::u64 m_timestampCPU = 0;
        core::u64 m_timestampGPU = 0;
		gfx::Buffer * m_queryBuffer = nullptr;
		core::uint m_queryIndex = 0;
		bool m_isValidQuery[MaxQueriesCount] = {};
	};
}