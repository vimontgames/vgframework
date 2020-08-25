#include "graphics/driver/Precomp.h"
#include "CommandPool.h"
#include "graphics/driver/Device/Device.h"

using namespace vg::core;

#include VG_GRAPHICSAPI_IMPLEMENTATION(CommandPool)

namespace vg::graphics::driver
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