#include "gfx/Precomp.h"
#include "CommandQueue.h"
#include "gfx/Device/Device.h"

#include VG_GFXAPI_IMPL(CommandQueue)

namespace vg::gfx
{
	using namespace core;

	namespace base
	{
		//--------------------------------------------------------------------------------------
		CommandQueue::CommandQueue(CommandQueueType _type) :
			m_type(_type)
		{
			string name = asString(_type) + "CmdQueue";
			SetName(name);
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