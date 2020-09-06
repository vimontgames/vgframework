#pragma once

#include "core/Object/Object.h"

namespace vg::graphics::driver
{
	namespace base
	{
		class CommandPool : public core::Object
		{
		public:
			CommandPool(core::uint _frame, core::uint _index);
			~CommandPool();

		private:
			core::u8 m_frame = (core::u8)-1;
			core::u8 m_index = (core::u8)-1;
		};
	}
}

#include VG_GFXAPI_HEADER(CommandPool)

namespace vg::graphics::driver
{
	class CommandPool : public VG_GFXAPI::CommandPool
	{
		using super = VG_GFXAPI::CommandPool;

	public:
		CommandPool(core::uint _frame, core::uint _index);
		~CommandPool();
	};
}