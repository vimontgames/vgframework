#pragma once

#include "core/Object/Object.h"

namespace vg::gfx
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

namespace vg::gfx
{
	class CommandPool : public VG_GFXAPI::CommandPool
	{
		using super = VG_GFXAPI::CommandPool;

	public:
        const char * getClassName() const final { return "CommandPool"; }

		CommandPool(core::uint _frame, core::uint _index);
		~CommandPool();
	};
}