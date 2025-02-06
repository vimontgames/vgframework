#pragma once

#include "CommandQueue_consts.h"
#include "core/Object/Object.h"

namespace vg::gfx
{
    constexpr core::uint TimestampQueryMaxCountPerFrame = 16;
	using TimestampQueryIndex = core::u16;

	namespace base
	{
		class CommandQueue : public core::Object
		{
		public:
			CommandQueue(CommandQueueType _type);
			~CommandQueue();

		private:
			CommandQueueType	m_type;
		};
	}
}

#ifdef VG_DX12
#include "dx12/CommandQueue_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/CommandQueue_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
	class CommandQueue : public VG_GFXAPI::CommandQueue
	{
		using super = VG_GFXAPI::CommandQueue;

	public:
        const char * GetClassName() const final { return "CommandQueue"; }

		CommandQueue(CommandQueueType _type);
		~CommandQueue();
	};
}