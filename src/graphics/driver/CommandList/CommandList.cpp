#include "graphics/driver/Precomp.h"
#include "CommandList.h"
#include "graphics/driver/CommandPool/CommandPool.h"
#include "graphics/driver/Device/Device.h"
#include "graphics/driver/RootSignature/RootSignature.h"
#include "graphics/driver/PipelineState/GraphicPipelineState.h"
#include "graphics/driver/FrameGraph/RenderPass.h"
#include "graphics/driver/FrameGraph/SubPass.h"
#include "graphics/driver/FrameGraph/UserPass.h"
#include "graphics/driver/Resource/Texture.h"
#include "graphics/driver/Resource/Buffer.h"
#include "graphics/driver/BindlessTable/BindlessTable.h"

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

            const RenderPassKey & key = _renderPass->getRenderPassKey();
            m_stateCache.graphicPipelineKey.m_renderPassKey = key;
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

            m_stateCache.graphicPipelineKey.m_subPassIndex = _subPassIndex;
            const SubPassKey & key = m_stateCache.graphicPipelineKey.m_renderPassKey.m_subPassKeys[_subPassIndex];

            uint width = 0, height = 0;

            for (uint i = 0; i < maxRenderTarget; ++i)
            {
                const auto flags = key.getRenderTargetFlags(i);
                if (SubPassKey::Flags::Bind & flags)
                {
                    const Texture * tex = m_renderPass->m_attachments[i];
                    auto & desc = tex->getTexDesc();
                    width = desc.width;
                    height = desc.height;
                }
            }

            if (SubPassKey::Flags::Bind & key.getDepthStencilFlags())
            {
                VG_ASSERT_NOT_IMPLEMENTED(); // separate depth stencil attachment ?
            }

            setViewport(uint4(0,0, width, height));
            setScissor(uint4(0, 0, width, height));
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

        //--------------------------------------------------------------------------------------
        void CommandList::setRootSignature(const RootSignatureHandle & _rsHandle)
        {
            if (_rsHandle != m_stateCache.graphicPipelineKey.m_rootSignature)
            {
                m_stateCache.dirtyFlags |= StateCache::DirtyFlags::RootSignature;
                m_stateCache.graphicPipelineKey.m_rootSignature = _rsHandle;
                m_currentRootSignature = driver::Device::get()->getRootSignature(_rsHandle);
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setPrimitiveTopology(PrimitiveTopology _topology)
        {
            if (_topology != m_stateCache.primitiveTopology)
            {
                m_stateCache.dirtyFlags |= StateCache::DirtyFlags::PrimitiveTopology;
                m_stateCache.primitiveTopology = _topology;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setViewport(const core::uint4 & _viewport)
        {
            if (any(_viewport != m_stateCache.viewport))
            {
                m_stateCache.dirtyFlags |= StateCache::DirtyFlags::Viewport;
                m_stateCache.viewport = _viewport;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setScissor(const core::uint4 & _scissor)
        {
            if (any(_scissor != m_stateCache.scissor))
            {
                m_stateCache.dirtyFlags |= StateCache::DirtyFlags::Scissor;
                m_stateCache.scissor = _scissor;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setRootConstants(core::uint _startOffset, core::u32 * _values, core::uint _count)
        {
            VG_ASSERT(_startOffset + _count < countof(m_stateCache.rootConstants));
            for (uint i = 0; i < _count; ++i)
            {
                if (_values[i] != m_stateCache.rootConstants[_startOffset + i])
                {
                    m_stateCache.rootConstants[_startOffset + i] = _values[i];
                    m_stateCache.dirtyFlags |= StateCache::DirtyFlags::RootConstants;
                }
            }
        }
        
        //--------------------------------------------------------------------------------------
        void CommandList::setShader(const ShaderKey & _key)
        {
            if (_key != m_stateCache.graphicPipelineKey.m_shaderKey)
            {
                m_stateCache.dirtyFlags |= StateCache::DirtyFlags::GraphicPipelineState;
                m_stateCache.graphicPipelineKey.m_shaderKey = _key;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setRasterizerState(const driver::RasterizerState & _rsState)
        {
            if (_rsState != m_stateCache.graphicPipelineKey.m_rasterizerState)
            {
                m_stateCache.dirtyFlags |= StateCache::DirtyFlags::GraphicPipelineState;
                m_stateCache.graphicPipelineKey.m_rasterizerState = _rsState;
            }
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
        for (auto & pair : m_graphicPipelineStateHash)
            VG_SAFE_RELEASE(pair.second);
        m_graphicPipelineStateHash.clear();
	}

    //--------------------------------------------------------------------------------------
    void CommandList::reset()
    {
        m_stateCache.dirtyFlags = (StateCache::DirtyFlags)-1;
        m_stateCache.primitiveTopology = (PrimitiveTopology)-1;
        m_stateCache.viewport = uint4(0, 0, 0, 0);
        super::reset();
    }

    //--------------------------------------------------------------------------------------
    void CommandList::close()
    {
        super::close();
    }

    //--------------------------------------------------------------------------------------
    void CommandList::flush()
    {
        if (asBool(m_stateCache.dirtyFlags))
        {
            auto * device = Device::get();

            if (asBool(StateCache::DirtyFlags::RootSignature & m_stateCache.dirtyFlags))
                super::bindRootSignature(device->getRootSignature(m_stateCache.graphicPipelineKey.m_rootSignature));

            if (asBool(StateCache::DirtyFlags::GraphicPipelineState & m_stateCache.dirtyFlags))
            {
                const auto & key = m_stateCache.graphicPipelineKey;
            
                GraphicPipelineState * pso = nullptr;
            
                auto it = m_graphicPipelineStateHash.find(key);
                if (m_graphicPipelineStateHash.end() == it)
                    m_graphicPipelineStateHash[key] = new GraphicPipelineState(key);
            
                pso = m_graphicPipelineStateHash[key];
                super::bindGraphicPipelineState(pso);
                m_stateCache.graphicPipelineKey = key;
            }
            
            if (asBool(StateCache::DirtyFlags::PrimitiveTopology & m_stateCache.dirtyFlags))
                super::bindPrimitiveTopology(m_stateCache.primitiveTopology);

            if (asBool(StateCache::DirtyFlags::Viewport & m_stateCache.dirtyFlags))
                super::bindViewport(m_stateCache.viewport);

            if (asBool(StateCache::DirtyFlags::Scissor & m_stateCache.dirtyFlags))
                super::bindScissor(m_stateCache.scissor);

            if (asBool(StateCache::DirtyFlags::RootConstants & m_stateCache.dirtyFlags))
                super::bindRootConstants(m_stateCache.rootConstants);

            m_stateCache.dirtyFlags = (StateCache::DirtyFlags)0;
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::draw(core::uint _vertexCount, core::uint _startOffset)
    {
        flush();

        super::draw(_vertexCount, _startOffset);
    }
}