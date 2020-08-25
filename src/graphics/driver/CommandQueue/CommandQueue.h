#pragma once

#include "CommandQueue_consts.h"
#include "core/Object/Object.h"

namespace vg::graphics::driver
{
	namespace base
	{
		class CommandQueue : public core::Object
		{
		public:
			CommandQueue(CommandQueueType _type, core::uint _index);
			~CommandQueue();

		private:
			CommandQueueType	m_type;
			core::uint			m_index;
		};
	}
}

#include VG_GRAPHICSAPI_HEADER(CommandQueue)

namespace vg::graphics::driver
{
	class CommandQueue : public VG_GRAPHICSAPI::CommandQueue
	{
		using super = VG_GRAPHICSAPI::CommandQueue;

	public:
		CommandQueue(CommandQueueType _type, core::uint _index);
		~CommandQueue();
	};
}