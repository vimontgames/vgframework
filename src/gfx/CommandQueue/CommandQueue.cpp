#include "gfx/Precomp.h"
#include "CommandQueue.h"
#include "gfx/Device/Device.h"

#ifdef VG_DX12
#include "dx12/CommandQueue_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/CommandQueue_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

using namespace vg::core;

namespace vg::gfx
{
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