#include "gfx/Precomp.h"
#include "CommandList.h"
#include "gfx/CommandPool/CommandPool.h"
#include "gfx/Device/Device.h"
#include "gfx/RootSignature/RootSignature.h"
#include "gfx/PipelineState/GraphicPipelineState.h"
#include "gfx/FrameGraph/RenderPass.h"
#include "gfx/FrameGraph/SubPass.h"
#include "gfx/FrameGraph/UserPass.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Resource/UploadBuffer.h"
#include "gfx/BindlessTable/BindlessTable.h"

using namespace vg::core;

#include VG_GFXAPI_IMPL(CommandList)

namespace vg::gfx
{
	namespace base
	{
		//--------------------------------------------------------------------------------------
		CommandList::CommandList(gfx::CommandListType _type, gfx::CommandPool * _cmdPool, core::uint _frame, core::uint _index) :
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
		gfx::CommandPool * CommandList::getCommandPool() const
		{ 
			return m_cmdPool; 
		}

		//--------------------------------------------------------------------------------------
		void CommandList::beginRenderPass(gfx::RenderPass * _renderPass)
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
		void CommandList::beginSubPass(core::uint _subPassIndex, gfx::SubPass * _subPass)
		{
			m_subPass = _subPass;
			m_subPassIndex = _subPassIndex;

            m_stateCache.graphicPipelineKey.m_subPassIndex = _subPassIndex;
            const SubPassKey & key = m_stateCache.graphicPipelineKey.m_renderPassKey.m_subPassKeys[_subPassIndex];

            uint width = 0, height = 0;

            for (uint i = 0; i < maxRenderTarget; ++i)
            {
                const auto & info = key.getColorAttachmentInfo(i);

                if (asBool(info.flags & SubPassKey::AttachmentFlags::RenderTarget))
                {
                    const Texture * tex = m_renderPass->m_colorAttachments[i]->getTexture();
                    auto & desc = tex->getTexDesc();

                    VG_ASSERT( !width || width  == desc.width);
                    VG_ASSERT(!height || height == desc.height);

                    width = desc.width;
                    height = desc.height;
                }
            }

            const auto & info = key.getDepthStencilAttachmentInfo();
            if (asBool(info.flags & SubPassKey::AttachmentFlags::RenderTarget))
            {
                const Texture * tex = m_renderPass->m_depthStencilAttachment->getTexture();
                auto & desc = tex->getTexDesc();

                VG_ASSERT( !width || width  == desc.width);
                VG_ASSERT(!height || height == desc.height);

                width = desc.width;
                height = desc.height;
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
		const gfx::RenderPass * CommandList::getRenderPass() const
		{
			return m_renderPass;
		}

        //--------------------------------------------------------------------------------------
        const gfx::SubPass * CommandList::getSubPass() const
        {
            return m_subPass;
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
                m_currentRootSignature = gfx::Device::get()->getRootSignature(_rsHandle);
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setPrimitiveTopology(PrimitiveTopology _topology)
        {
            if (_topology != m_stateCache.graphicPipelineKey.m_primitiveTopology)
            {
                m_stateCache.dirtyFlags |= StateCache::DirtyFlags::GraphicPipelineState;
                m_stateCache.graphicPipelineKey.m_primitiveTopology = _topology;
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
        void CommandList::setInlineRootConstants(const void * _value, core::uint _count)
        {
            setRootConstants(0, (u32*)_value, _count);
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setIndexBuffer(gfx::Buffer * _ib)
        {
            if (_ib != m_stateCache.indexBuffer)
            {
                VG_ASSERT(!_ib || 2 == _ib->getBufDesc().elementSize || 4 == _ib->getBufDesc().elementSize, "Invalid Index Buffer element size %u (should 2 or 4)", _ib->getBufDesc().elementSize);
                m_stateCache.dirtyFlags |= StateCache::DirtyFlags::IndexBuffer;
                m_stateCache.indexBuffer = _ib;
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
        void CommandList::setRasterizerState(const gfx::RasterizerState & _rs)
        {
            if (_rs != m_stateCache.graphicPipelineKey.m_rasterizerState)
            {
                m_stateCache.dirtyFlags |= StateCache::DirtyFlags::GraphicPipelineState;
                m_stateCache.graphicPipelineKey.m_rasterizerState = _rs;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setDepthStencilState(const gfx::DepthStencilState & _ds)
        {
            if (_ds != m_stateCache.graphicPipelineKey.m_depthStencilState)
            {
                m_stateCache.dirtyFlags |= StateCache::DirtyFlags::GraphicPipelineState;
                m_stateCache.graphicPipelineKey.m_depthStencilState = _ds;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setBlendState(const gfx::BlendState & _bs)
        {
            if (_bs != m_stateCache.graphicPipelineKey.m_blendState)
            {
                m_stateCache.dirtyFlags |= StateCache::DirtyFlags::GraphicPipelineState;
                m_stateCache.graphicPipelineKey.m_blendState = _bs;
            }
        }
	}

	//--------------------------------------------------------------------------------------
	CommandList::CommandList(gfx::CommandListType _type, gfx::CommandPool * _cmdPool, core::uint _frame, core::uint _index) :
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
        m_stateCache.viewport = uint4(0, 0, 0, 0);
        m_stateCache.indexBuffer = nullptr;
        memset(m_stateCache.rootConstants, 0x00000000, sizeof(m_stateCache.rootConstants));
        super::reset();
    }

    //--------------------------------------------------------------------------------------
    void CommandList::close()
    {
        super::close();
    }

    //--------------------------------------------------------------------------------------
    void CommandList::resetShaders(ShaderKey::File _file)
    {
        vector<GraphicPipelineStateKey> keys;

        for (auto & pair : m_graphicPipelineStateHash)
        {
            const GraphicPipelineStateKey & key = pair.first;
            if (key.m_shaderKey.file == _file)
            {
                VG_SAFE_RELEASE(pair.second);
                keys.push_back(key);
            }
        }

        for (GraphicPipelineStateKey key : keys)
            m_graphicPipelineStateHash.erase(key);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::flush()
    {
        if (asBool(m_stateCache.dirtyFlags))
        {
            auto * device = Device::get();

            if (asBool(StateCache::DirtyFlags::RootSignature & m_stateCache.dirtyFlags))
                super::bindRootSignature(device->getRootSignature(m_stateCache.graphicPipelineKey.m_rootSignature));

            if (asBool(StateCache::DirtyFlags::IndexBuffer & m_stateCache.dirtyFlags))
                super::bindIndexBuffer(m_stateCache.indexBuffer);

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

                super::bindPrimitiveTopology(m_stateCache.graphicPipelineKey.m_primitiveTopology);
            }

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
    void CommandList::draw(uint _vertexCount, uint _startOffset)
    {
        flush();

        super::draw(_vertexCount, _startOffset);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::drawIndexed(uint _indexCount, uint _startIndex, uint _baseVertex)
    {
        flush();

        super::drawIndexed(_indexCount, _startIndex, _baseVertex);
    }
}