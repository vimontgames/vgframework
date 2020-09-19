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
		CommandQueue::CommandQueue(CommandQueueType _type) :
			m_type(_type)
		{
			string name = asString(_type) + "CmdQueue";
			setName(name);
		}

		//--------------------------------------------------------------------------------------
		CommandQueue::~CommandQueue()
		{

		}
	}

	//--------------------------------------------------------------------------------------
	CommandQueue::CommandQueue(CommandQueueType _type) :
		super::CommandQueue(_type)
	{

	}

	//--------------------------------------------------------------------------------------
	CommandQueue::~CommandQueue()
	{
	
	}
}