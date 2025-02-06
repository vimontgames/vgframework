#include "gfx/Precomp.h"
#include "CommandPool.h"
#include "gfx/Device/Device.h"

#ifdef VG_DX12
#include "dx12/CommandPool_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/CommandPool_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

using namespace vg::core;

namespace vg::gfx
{
	namespace base
	{
		//--------------------------------------------------------------------------------------
		CommandPool::CommandPool(core::uint _frame, core::uint _index) :
			m_frame(_frame),
			m_index(_index)
		{

		}

		//--------------------------------------------------------------------------------------
		CommandPool::~CommandPool()
		{

		}
	}

	//--------------------------------------------------------------------------------------
	CommandPool::CommandPool(core::uint _frame, core::uint _index) :
		super::CommandPool(_frame, _index)
	{

	}

	//--------------------------------------------------------------------------------------
	CommandPool::~CommandPool()
	{

	}
}