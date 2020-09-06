#include "graphics/driver/Precomp.h"
#include "CommandQueue.h"
#include "graphics/driver/Device/Device.h"

#include VG_GFXAPI_IMPL(CommandQueue)

namespace vg::graphics::driver
{
	using namespace core;

	namespace base
	{
		//--------------------------------------------------------------------------------------
		CommandQueue::CommandQueue(CommandQueueType _type, core::uint _index) :
			m_type(_type),
			m_index(_index)
		{
			string name = asString(_type) + "CmdQueue#" + std::to_string(_index);
			setName(name);
		}

		//--------------------------------------------------------------------------------------
		CommandQueue::~CommandQueue()
		{

		}
	}

	//--------------------------------------------------------------------------------------
	CommandQueue::CommandQueue(CommandQueueType _type, core::uint _index) :
		super::CommandQueue(_type, _index)
	{

	}

	//--------------------------------------------------------------------------------------
	CommandQueue::~CommandQueue()
	{
	
	}
}