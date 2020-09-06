#include "graphics/driver/Precomp.h"
#include "CommandList.h"
#include "graphics/driver/CommandPool/CommandPool.h"
#include "graphics/driver/Device/Device.h"
#include "graphics/driver/FrameGraph/RenderPass.h"
#include "graphics/driver/FrameGraph/SubPass.h"
#include "graphics/driver/Texture/Texture.h"

using namespace vg::core;

#include VG_GFXAPI_IMPL(CommandList)

namespace vg::graphics::driver
{
	namespace base
	{
		//--------------------------------------------------------------------------------------
		CommandList::CommandList(driver::CommandListType _type, driver::CommandPool * _cmdPool, core::uint _frame, core::uint _index) :
			m_type(_type),
			m_frame(_frame),
			m_index(_index),
			m_cmdPool(_cmdPool)
		{
			string name = asString(_type) + "CmdList#" + to_string(_index);
			setName(name);
		}

		//--------------------------------------------------------------------------------------
		CommandList::~CommandList()
		{

		}

		//--------------------------------------------------------------------------------------
		CommandListType CommandList::getType() const 
		{ 
			return m_type; 
		}

		//--------------------------------------------------------------------------------------
		driver::CommandPool * CommandList::getCommandPool() 
		{ 
			return m_cmdPool; 
		}

		//--------------------------------------------------------------------------------------
		void CommandList::beginRenderPass(const driver::RenderPass * _renderPass)
		{
			m_renderPass = _renderPass;
		}

		//--------------------------------------------------------------------------------------
		void CommandList::endRenderPass()
		{
			m_renderPass = nullptr;
		}

		//--------------------------------------------------------------------------------------
		void CommandList::beginSubPass(core::uint _subPassIndex, const driver::SubPass * _subPass)
		{
			m_subPass = _subPass;
			m_subPassIndex = _subPassIndex;
		}

		//--------------------------------------------------------------------------------------
		void CommandList::endSubPass()
		{
			m_subPass = nullptr;
			m_subPassIndex = -1;
		}

		//--------------------------------------------------------------------------------------
		const driver::RenderPass * CommandList::getRenderPass() const
		{
			return m_renderPass;
		}

		//--------------------------------------------------------------------------------------
		const core::uint CommandList::getSubPassIndex() const
		{
			return m_subPassIndex;
		}
	}

	//--------------------------------------------------------------------------------------
	CommandList::CommandList(driver::CommandListType _type, driver::CommandPool * _cmdPool, core::uint _frame, core::uint _index) :
		super::CommandList(_type, _cmdPool, _frame, _index)
	{

	}

	//--------------------------------------------------------------------------------------
	CommandList::~CommandList()
	{

	}

    //--------------------------------------------------------------------------------------
    void CommandList::reset()
    {
        super::reset();
    }

    //--------------------------------------------------------------------------------------
    void CommandList::close()
    {
        super::close();
    }
}