#include "gfx/Precomp.h"
#include "CommandList.h"
#include "gfx/CommandPool/CommandPool.h"
#include "gfx/Device/Device.h"
#include "gfx/RootSignature/RootSignature.h"
#include "gfx/PipelineState/Graphic/GraphicPipelineState.h"
#include "gfx/PipelineState/Compute/ComputePipelineState.h"
#include "gfx/FrameGraph/RenderPass.h"
#include "gfx/FrameGraph/SubPass.h"
#include "gfx/FrameGraph/UserPass.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/RingBuffer/Upload/UploadBuffer.h"
#include "gfx/BindlessTable/BindlessTable.h"

using namespace vg::core;

#include VG_GFXAPI_IMPL(CommandList)

#if !VG_ENABLE_INLINE
#include "CommandList.inl"
#endif

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
            m_graphicStateCache.m_graphicPipelineKey.m_renderPassKey = key;
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

            m_graphicStateCache.m_graphicPipelineKey.m_subPassIndex = _subPassIndex;
            const SubPassKey & key = m_graphicStateCache.m_graphicPipelineKey.m_renderPassKey.m_subPassKeys[_subPassIndex];

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
                const Texture * tex = m_renderPass->getDepthStencilAttachment()->getTexture();
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
        void CommandList::setGraphicRootSignature(const RootSignatureHandle & _rsHandle)
        {
            if (_rsHandle != m_graphicStateCache.m_graphicPipelineKey.m_rootSignature)
            {
                m_graphicStateCache.m_dirtyFlags |= GraphicStateCache::DirtyFlags::RootSignature;
                m_graphicStateCache.m_graphicPipelineKey.m_rootSignature = _rsHandle;
                m_currentGraphicRootSignature = gfx::Device::get()->getRootSignature(_rsHandle);
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setPrimitiveTopology(PrimitiveTopology _topology)
        {
            if (_topology != m_graphicStateCache.m_graphicPipelineKey.m_primitiveTopology)
            {
                m_graphicStateCache.m_dirtyFlags |= GraphicStateCache::DirtyFlags::PipelineState;
                m_graphicStateCache.m_graphicPipelineKey.m_primitiveTopology = _topology;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setViewport(const core::uint4 & _viewport)
        {
            if (any(_viewport != m_graphicStateCache.m_viewport))
            {
                m_graphicStateCache.m_dirtyFlags |= GraphicStateCache::DirtyFlags::Viewport;
                m_graphicStateCache.m_viewport = _viewport;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setScissor(const core::uint4 & _scissor)
        {
            if (any(_scissor != m_graphicStateCache.m_scissor))
            {
                m_graphicStateCache.m_dirtyFlags |= GraphicStateCache::DirtyFlags::Scissor;
                m_graphicStateCache.m_scissor = _scissor;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setGraphicRootConstants(core::uint _startOffset, core::u32 * _values, core::uint _count)
        {
            VG_ASSERT(_startOffset + _count < countof(m_graphicStateCache.m_rootConstants));
            for (uint i = 0; i < _count; ++i)
            {
                if (_values[i] != m_graphicStateCache.m_rootConstants[_startOffset + i])
                {
                    m_graphicStateCache.m_rootConstants[_startOffset + i] = _values[i];
                    m_graphicStateCache.m_dirtyFlags |= GraphicStateCache::DirtyFlags::RootConstants;
                }
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setIndexBuffer(gfx::Buffer * _ib)
        {
            if (_ib != m_graphicStateCache.m_indexBuffer)
            {
                VG_ASSERT(!_ib || 2 == _ib->getBufDesc().elementSize || 4 == _ib->getBufDesc().elementSize, "Invalid Index Buffer element size %u (should 2 or 4)", _ib->getBufDesc().elementSize);
                m_graphicStateCache.m_dirtyFlags |= GraphicStateCache::DirtyFlags::IndexBuffer;
                m_graphicStateCache.m_indexBuffer = _ib;
            }
        }
        
        //--------------------------------------------------------------------------------------
        void CommandList::setShader(const ShaderKey & _key)
        {
            if (_key != m_graphicStateCache.m_graphicPipelineKey.m_shaderKey)
            {
                m_graphicStateCache.m_dirtyFlags |= GraphicStateCache::DirtyFlags::PipelineState;
                m_graphicStateCache.m_graphicPipelineKey.m_shaderKey = _key;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setRasterizerState(const gfx::RasterizerState & _rs)
        {
            if (_rs != m_graphicStateCache.m_graphicPipelineKey.m_rasterizerState)
            {
                m_graphicStateCache.m_dirtyFlags |= GraphicStateCache::DirtyFlags::PipelineState;
                m_graphicStateCache.m_graphicPipelineKey.m_rasterizerState = _rs;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setDepthStencilState(const gfx::DepthStencilState & _ds)
        {
            if (_ds != m_graphicStateCache.m_graphicPipelineKey.m_depthStencilState)
            {
                m_graphicStateCache.m_dirtyFlags |= GraphicStateCache::DirtyFlags::PipelineState;
                m_graphicStateCache.m_graphicPipelineKey.m_depthStencilState = _ds;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setBlendState(const gfx::BlendState & _bs)
        {
            if (_bs != m_graphicStateCache.m_graphicPipelineKey.m_blendState)
            {
                m_graphicStateCache.m_dirtyFlags |= GraphicStateCache::DirtyFlags::PipelineState;
                m_graphicStateCache.m_graphicPipelineKey.m_blendState = _bs;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setComputeRootSignature(const RootSignatureHandle & _rsHandle)
        {
            if (_rsHandle != m_computeStateCache.m_computePipelineKey.m_computeRootSignature)
            {
                m_computeStateCache.m_dirtyFlags |= ComputeStateCache::DirtyFlags::RootSignature;
                m_computeStateCache.m_computePipelineKey.m_computeRootSignature = _rsHandle;
                m_currentComputeRootSignature = gfx::Device::get()->getRootSignature(_rsHandle);
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setComputeShader(const ComputeShaderKey & _computeKey)
        {
            if (_computeKey != m_computeStateCache.m_computePipelineKey.m_computeShaderKey)
            {
                m_computeStateCache.m_dirtyFlags |= ComputeStateCache::DirtyFlags::PipelineState;
                m_computeStateCache.m_computePipelineKey.m_computeShaderKey = _computeKey;
            }
        }

        //--------------------------------------------------------------------------------------
        void CommandList::setComputeRootConstants(core::uint _startOffset, core::u32 * _values, core::uint _count)
        {
            VG_ASSERT(_startOffset + _count < countof(m_computeStateCache.m_rootConstants));
            for (uint i = 0; i < _count; ++i)
            {
                if (_values[i] != m_computeStateCache.m_rootConstants[_startOffset + i])
                {
                    m_computeStateCache.m_rootConstants[_startOffset + i] = _values[i];
                    m_computeStateCache.m_dirtyFlags |= ComputeStateCache::DirtyFlags::RootConstants;
                }
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

        for (auto & pair : m_computePipelineStateHash)
            VG_SAFE_RELEASE(pair.second);
        m_computePipelineStateHash.clear();
	}

    //--------------------------------------------------------------------------------------
    void CommandList::reset()
    {
        memset(&m_graphicStateCache.m_graphicPipelineKey, -1, sizeof(GraphicPipelineStateKey));
        m_graphicStateCache.m_dirtyFlags = (GraphicStateCache::DirtyFlags)-1;
        m_graphicStateCache.m_viewport = uint4(0, 0, 0, 0);
        m_graphicStateCache.m_indexBuffer = nullptr;
        memset(m_graphicStateCache.m_rootConstants, 0x0, sizeof(m_graphicStateCache.m_rootConstants));

        memset(&m_computeStateCache.m_computePipelineKey, -1, sizeof(ComputePipelineStateKey));
        m_computeStateCache.m_dirtyFlags = (ComputeStateCache::DirtyFlags)-1;
        memset(m_computeStateCache.m_rootConstants, 0x0, sizeof(m_computeStateCache.m_rootConstants));

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
                VG_SAFE_RELEASE_ASYNC(pair.second);
                keys.push_back(key);
            }
        }
        for (GraphicPipelineStateKey key : keys)
            m_graphicPipelineStateHash.erase(key);

        vector<ComputePipelineStateKey> computeKeys;
        for (auto & pair : m_computePipelineStateHash)
        {
            const ComputePipelineStateKey & computeKey = pair.first;
            if (computeKey.m_computeShaderKey.file == _file)
            {
                VG_SAFE_RELEASE_ASYNC(pair.second);
                computeKeys.push_back(computeKey);
            }
        }
        for (ComputePipelineStateKey computeKey : computeKeys)
            m_computePipelineStateHash.erase(computeKey);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::beginRenderPass(gfx::RenderPass * _renderPass)
    {
        super::beginRenderPass(_renderPass);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::endRenderPass()
    {
        super::endRenderPass();
    }

    //--------------------------------------------------------------------------------------
    bool CommandList::getGraphicPipelineState(const GraphicPipelineStateKey & _key, GraphicPipelineState *& _graphicPipelineState)
    {
        GraphicPipelineState * pso = nullptr;

        auto it = m_graphicPipelineStateHash.find(_key);
        if (m_graphicPipelineStateHash.end() != it)
        {
            pso = it->second;
        }
        else
        {
            pso = GraphicPipelineState::createGraphicPipelineState(_key);
            m_graphicPipelineStateHash[_key] = pso;
        }
        
        _graphicPipelineState = pso;

        return nullptr != pso;
    }

    //--------------------------------------------------------------------------------------
    bool CommandList::getComputePipelineState(const ComputePipelineStateKey & _key, ComputePipelineState *& _computePipelineState)
    {
        ComputePipelineState * pso = nullptr;

        auto it = m_computePipelineStateHash.find(_key);
        if (m_computePipelineStateHash.end() != it)
        {
            pso = it->second;
        }
        else
        {
            pso = ComputePipelineState::createComputePipelineState(_key);
            m_computePipelineStateHash[_key] = pso;
        }

        _computePipelineState = pso;

        return nullptr != pso;
    }

    //--------------------------------------------------------------------------------------
    bool CommandList::applyGraphicPipelineState()
    {
        if (asBool(m_graphicStateCache.m_dirtyFlags))
        {
            auto * device = Device::get();

            if (asBool(GraphicStateCache::DirtyFlags::RootSignature & m_graphicStateCache.m_dirtyFlags))
                super::bindGraphicRootSignature(device->getRootSignature(m_graphicStateCache.m_graphicPipelineKey.m_rootSignature));

            if (asBool(GraphicStateCache::DirtyFlags::IndexBuffer & m_graphicStateCache.m_dirtyFlags))
                super::bindIndexBuffer(m_graphicStateCache.m_indexBuffer);

            if (asBool(GraphicStateCache::DirtyFlags::PipelineState & m_graphicStateCache.m_dirtyFlags))
            {
                const auto & key = m_graphicStateCache.m_graphicPipelineKey;
            
                GraphicPipelineState * pso = nullptr;
                if (!getGraphicPipelineState(key, pso))
                    return false;
            
                super::bindGraphicPipelineState(pso);
                m_graphicStateCache.m_graphicPipelineKey = key;

                super::bindPrimitiveTopology(m_graphicStateCache.m_graphicPipelineKey.m_primitiveTopology);
            }

            if (asBool(GraphicStateCache::DirtyFlags::Viewport & m_graphicStateCache.m_dirtyFlags))
                super::bindViewport(m_graphicStateCache.m_viewport);

            if (asBool(GraphicStateCache::DirtyFlags::Scissor & m_graphicStateCache.m_dirtyFlags))
                super::bindScissor(m_graphicStateCache.m_scissor);

            if (asBool(GraphicStateCache::DirtyFlags::RootConstants & m_graphicStateCache.m_dirtyFlags))
                super::bindGraphicRootConstants(m_graphicStateCache.m_rootConstants);

            m_graphicStateCache.m_dirtyFlags = (GraphicStateCache::DirtyFlags)0;
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    void CommandList::draw(uint _vertexCount, uint _startOffset)
    {
        if (applyGraphicPipelineState())
            super::draw(_vertexCount, _startOffset);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::drawIndexed(uint _indexCount, uint _startIndex, uint _baseVertex)
    {
        if (applyGraphicPipelineState())
            super::drawIndexed(_indexCount, _startIndex, _baseVertex);
    }

    //--------------------------------------------------------------------------------------
    bool CommandList::applyComputePipelineState()
    {
        if (asBool(m_computeStateCache.m_dirtyFlags))
        {
            auto * device = Device::get();

            if (asBool(ComputeStateCache::DirtyFlags::RootSignature & m_computeStateCache.m_dirtyFlags))
                super::bindComputeRootSignature(device->getRootSignature(m_computeStateCache.m_computePipelineKey.m_computeRootSignature));

            if (asBool(ComputeStateCache::DirtyFlags::PipelineState & m_computeStateCache.m_dirtyFlags))
            {
                const auto & key = m_computeStateCache.m_computePipelineKey;

                ComputePipelineState * pso = nullptr;
                if (!getComputePipelineState(key, pso))
                    return false;

                super::bindComputePipelineState(pso);
                m_computeStateCache.m_computePipelineKey = key;
            }

            if (asBool(ComputeStateCache::DirtyFlags::RootConstants & m_computeStateCache.m_dirtyFlags))
                super::bindComputeRootConstants(m_computeStateCache.m_rootConstants);

            m_computeStateCache.m_dirtyFlags = (ComputeStateCache::DirtyFlags)0;
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    void CommandList::dispatch(core::uint3 _threadGroupCount)
    {
        if (applyComputePipelineState())
            super::dispatch(_threadGroupCount.xyz);
    }
}