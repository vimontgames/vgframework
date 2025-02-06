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

#ifdef VG_DX12
#include "dx12/CommandPool_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/CommandPool_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
	class CommandPool : public VG_GFXAPI::CommandPool
	{
		using super = VG_GFXAPI::CommandPool;

	public:
        const char * GetClassName() const final { return "CommandPool"; }

		CommandPool(core::uint _frame, core::uint _index);
		~CommandPool();
	};
}