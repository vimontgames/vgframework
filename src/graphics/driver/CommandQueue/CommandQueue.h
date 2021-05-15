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
			CommandQueue(CommandQueueType _type);
			~CommandQueue();

		private:
			CommandQueueType	m_type;
		};
	}
}

#include VG_GFXAPI_HEADER(CommandQueue)

namespace vg::graphics::driver
{
	class CommandQueue : public VG_GFXAPI::CommandQueue
	{
		using super = VG_GFXAPI::CommandQueue;

	public:
        const char * getClassName() const final { return "CommandQueue"; }

		CommandQueue(CommandQueueType _type);
		~CommandQueue();
	};
}