#include "core/Math/Math.h"
#include "gfx/Device/Device.h"

namespace vg::gfx::dx12
{
	//--------------------------------------------------------------------------------------
	CommandList::CommandList(gfx::CommandListType _type, gfx::CommandPool * _cmdPool, core::uint _frame, core::uint _index) :
		super::CommandList(_type, _cmdPool, _frame, _index)
	{
		Device * device = gfx::Device::get();
		auto * d3d12device = device->getd3d12Device();

		switch (getType())
		{
			case CommandListType::Graphics:
				VG_ASSERT_SUCCEEDED(d3d12device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdPool->getd3d12CommandAllocator(), nullptr, IID_PPV_ARGS(&m_d3d12graphicsCmdList)));
				break;

			default:
				VG_ASSERT_NOT_IMPLEMENTED();
				break;
		}	
		close();
	}

	//--------------------------------------------------------------------------------------
	CommandList::~CommandList()
	{
		VG_SAFE_RELEASE(m_d3d12cmdList);
	}

	//--------------------------------------------------------------------------------------
	D3D12GraphicsCommandList * CommandList::getd3d12GraphicsCommandList()
	{
		VG_ASSERT(CommandListType::Graphics == getType());
		return m_d3d12graphicsCmdList;
	}

	//--------------------------------------------------------------------------------------
	void CommandList::reset()
	{
		switch (getType())
		{
			case CommandListType::Graphics:
                VG_ASSERT_SUCCEEDED(m_d3d12graphicsCmdList->Reset(getCommandPool()->getd3d12CommandAllocator(), nullptr));
            break;

			default:
				VG_ASSERT_NOT_IMPLEMENTED();
            break;
		}

        auto * device = gfx::Device::get();
        ID3D12DescriptorHeap * d3d12descriptorHeap = device->getBindlessTable()->getd3d12GPUDescriptorHeap();
        m_d3d12graphicsCmdList->SetDescriptorHeaps(1, &d3d12descriptorHeap);
	}

	//--------------------------------------------------------------------------------------
	void CommandList::close()
	{
		switch (getType())
		{
			case CommandListType::Graphics:
				VG_ASSERT_SUCCEEDED(m_d3d12graphicsCmdList->Close());
				break;

			default:
				VG_ASSERT_NOT_IMPLEMENTED();
				break;
		}
	}

    //--------------------------------------------------------------------------------------
    D3D12_RESOURCE_STATES getd3d12ResourceBarrierType(ResourceState _state)
    {
        switch (_state)
        {
            default:
                VG_ASSERT(false, "Unhandled ResourceState \"%s\" (%u)", asString(_state).c_str(), _state);

            case ResourceState::Undefined:
                return D3D12_RESOURCE_STATE_COMMON;

            case ResourceState::UnorderedAccess:
                return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

            case ResourceState::RenderTarget:
                return D3D12_RESOURCE_STATE_RENDER_TARGET;

            case ResourceState::ShaderResource:
                return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        }
    }

	//--------------------------------------------------------------------------------------
	void CommandList::beginRenderPass(gfx::RenderPass * _renderPass)
	{
		super::beginRenderPass(_renderPass);

		// [...]
	}

	//--------------------------------------------------------------------------------------
	void CommandList::endRenderPass()
	{
		// [...]

		super::endRenderPass();
	}

	//--------------------------------------------------------------------------------------
	void CommandList::beginSubPass(core::uint _subPassIndex, gfx::SubPass * _subPass)
	{
		super::beginSubPass(_subPassIndex, _subPass);

        if (_subPass)
        {
            const RenderPass * renderPass = getRenderPass();
            const RenderPassKey & renderPasskey = renderPass->getRenderPassKey();
            const auto index = getSubPassIndex();
            const SubPassKey & subPassKey = renderPasskey.m_subPassKeys[index];
            const auto & attachments = renderPass->m_colorAttachments;
            
            auto passInfo = _subPass->getUserPassesInfos()[0];
            auto userPass = passInfo.m_userPass;

            for (uint i = 0; i < _subPass->m_renderTargetCount; ++i)
            {
                const ResourceTransitionDesc & info = subPassKey.getColorAttachmentInfo(i);
                if (asBool(ResourceTransitionFlags::RenderTarget & info.flags))
                {
                    const FrameGraphTextureResource * res = userPass->getRenderTargets()[i];
                    const FrameGraphTextureResourceDesc & resourceDesc = res->getTextureResourceDesc();
                    D3D12_RENDER_PASS_RENDER_TARGET_DESC & renderTargetDesc = _subPass->m_d3d12renderPassRenderTargetDesc[i];
                    const Texture * tex = res->getTexture();
                    _subPass->m_d3d12renderPassRenderTargetDesc[i].cpuDescriptor = tex->getd3d12RTVHandle();
                }
            }      

            if (_subPass->m_depthStencilCount > 0)
            {
                const ResourceTransitionDesc & info = subPassKey.getDepthStencilAttachmentInfo();
                if (asBool(ResourceTransitionFlags::RenderTarget & info.flags))
                {
                    const FrameGraphTextureResource * res = userPass->getDepthStencil();
                    const FrameGraphTextureResourceDesc & resourceDesc = res->getTextureResourceDesc();
                    D3D12_RENDER_PASS_DEPTH_STENCIL_DESC & depthStencilDesc = _subPass->m_d3d12renderPassDepthStencilDesc;
                    const Texture * tex = res->getTexture();
                    _subPass->m_d3d12renderPassDepthStencilDesc.cpuDescriptor = tex->getd3d12DSVHandle();
                }
            }

            vector<D3D12_RESOURCE_BARRIER> d3d12barriers;

            const auto resourceTransitions = passInfo.m_resourceTransitions;
            for (uint i = 0; i < resourceTransitions.size(); ++i)
            {
                const auto resTrans = resourceTransitions[i];

                D3D12_RESOURCE_BARRIER d3d12barrier;

                const auto resType = resTrans.m_resource->getType();
                switch (resType)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(resType);
                        break;

                    case FrameGraphResource::Type::Texture:
                        d3d12barrier.Transition.pResource = ((FrameGraphTextureResource*)resTrans.m_resource)->getTexture()->getResource().getd3d12TextureResource();
                        d3d12barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                        d3d12barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                        d3d12barrier.Transition.StateBefore = getd3d12ResourceBarrierType(resTrans.m_transitionDesc.begin);
                        d3d12barrier.Transition.StateAfter = getd3d12ResourceBarrierType(resTrans.m_transitionDesc.end);
                        d3d12barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

                        d3d12barriers.push_back(d3d12barrier);
                        break;
                }
            }

            if (d3d12barriers.size() > 0)
                m_d3d12graphicsCmdList->ResourceBarrier((uint)d3d12barriers.size(), d3d12barriers.data());

            if (RenderPassType::Graphic == m_renderPass->getRenderPassType())
                m_d3d12graphicsCmdList->BeginRenderPass(_subPass->m_renderTargetCount, _subPass->m_renderTargetCount ? _subPass->m_d3d12renderPassRenderTargetDesc : nullptr, _subPass->m_depthStencilCount ? &_subPass->m_d3d12renderPassDepthStencilDesc : nullptr, _subPass->m_d3d12renderPassFlags);
        }
	}

    //--------------------------------------------------------------------------------------
    void CommandList::transitionResource(gfx::Texture * _texture, ResourceState _before, ResourceState _after)
    {
        D3D12_RESOURCE_BARRIER barrier;
        barrier.Transition.pResource = _texture->getResource().getd3d12TextureResource();
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.StateBefore = getd3d12ResourceBarrierType(_before);
        barrier.Transition.StateAfter = getd3d12ResourceBarrierType(_after);;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        m_d3d12graphicsCmdList->ResourceBarrier(1, &barrier);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::transitionResource(gfx::Buffer * _buffer, ResourceState _before, ResourceState _after)
    {
        D3D12_RESOURCE_BARRIER barrier;
        barrier.Transition.pResource = _buffer->getResource().getd3d12BufferResource();
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.StateBefore = getd3d12ResourceBarrierType(_before);
        barrier.Transition.StateAfter = getd3d12ResourceBarrierType(_after);;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        m_d3d12graphicsCmdList->ResourceBarrier(1, &barrier);
    }

    //--------------------------------------------------------------------------------------
	void CommandList::endSubPass()
	{
        if (RenderPassType::Graphic == m_renderPass->getRenderPassType())
		    m_d3d12graphicsCmdList->EndRenderPass();

        const RenderPass * renderPass = getRenderPass();
        const SubPass * subPass = getSubPass();

        const RenderPassKey & renderPasskey = renderPass->getRenderPassKey();
        const auto index = getSubPassIndex();
        const SubPassKey &  subPassKey = renderPasskey.m_subPassKeys[index];

        auto userPass = subPass->getUserPassesInfos()[0].m_userPass;

        vector<D3D12_RESOURCE_BARRIER> d3d12barriers;

        for (uint i = 0; i < maxRenderTarget; ++i)
        {
            const ResourceTransitionDesc & info = subPassKey.getColorAttachmentInfo(i);

            if (info.begin != info.end)
            {
                auto & res = userPass->getRenderTargets()[i];
                gfx::Texture * tex = res->getTexture();

                // skip the 1st and last pass for backbuffer (PRESENT=>RT & RT=>PRESENT) as they are handled by the device itself
                if (tex->getTexDesc().isBackbuffer())
                {
                    if (res->isFirstWrite(userPass) || res->isLastWrite(userPass))
                        continue;
                }

                D3D12_RESOURCE_BARRIER d3d12barrier;

                d3d12barrier.Transition.pResource = tex->getResource().getd3d12TextureResource(); 
                d3d12barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                d3d12barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                d3d12barrier.Transition.StateBefore = getd3d12ResourceBarrierType(info.begin);
                d3d12barrier.Transition.StateAfter = getd3d12ResourceBarrierType(info.end);
                d3d12barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

                d3d12barriers.push_back(d3d12barrier);
            }
        }

        auto rwTextures = userPass->getRWTextures();
        for (uint i = 0; i < rwTextures.size(); ++i)
        {
            const auto & res = rwTextures[i];

            //switch (res->getCurrentState())
            //{
            //    case ResourceState::ShaderResource:
            //    {
            //        //// UAV barrier
            //        //{
            //        //    D3D12_RESOURCE_BARRIER d3d12barrier = {};
            //        //    d3d12barrier.Transition.pResource = res->getTexture()->getResource().getd3d12TextureResource();
            //        //    d3d12barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
            //        //    d3d12barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            //        //
            //        //    d3d12barriers.push_back(d3d12barrier);
            //        //}
            //        //
            //        //// UAV to ShaderResource barrier
            //        //{
            //        //    D3D12_RESOURCE_BARRIER d3d12barrier;
            //        //
            //        //    d3d12barrier.Transition.pResource = res->getTexture()->getResource().getd3d12TextureResource();
            //        //    d3d12barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            //        //    d3d12barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            //        //    d3d12barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            //        //    d3d12barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            //        //    d3d12barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            //        //
            //        //    d3d12barriers.push_back(d3d12barrier);
            //        //}                    
            //    }
            //    break;
            //}
        }

        if (d3d12barriers.size() > 0)
            m_d3d12graphicsCmdList->ResourceBarrier((uint)d3d12barriers.size(), d3d12barriers.data());

		super::endSubPass();
	}

    //--------------------------------------------------------------------------------------
    void CommandList::bindIndexBuffer(gfx::Buffer * _ib)
    {
        D3D12_INDEX_BUFFER_VIEW d3d12IBView = {};

        if (_ib)
        {
            const BufferDesc & ibDesc = _ib->getBufDesc();

            d3d12IBView.BufferLocation = _ib->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();
            d3d12IBView.Format = ibDesc.elementSize == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
            d3d12IBView.SizeInBytes = ibDesc.size();
        }

        m_d3d12graphicsCmdList->IASetIndexBuffer(&d3d12IBView);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindGraphicRootSignature(gfx::RootSignature * _rootSig)
    {
        auto * d3d12rs = _rootSig->getd3d12RootSignature();
        m_d3d12graphicsCmdList->SetGraphicsRootSignature(d3d12rs);
        m_d3d12graphicsCmdList->SetGraphicsRootDescriptorTable(1, gfx::Device::get()->getBindlessTable()->getd3d12GPUDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindGraphicPipelineState(gfx::GraphicPipelineState * _pso)
    {
        auto * d3s12pso = _pso->getd3d12GraphicPipelineState();
        m_d3d12graphicsCmdList->SetPipelineState(d3s12pso);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindPrimitiveTopology(PrimitiveTopology _topology)
    {
        auto d3d12primitiveTopology = getd3d12PrimitiveTopology(_topology);
        m_d3d12graphicsCmdList->IASetPrimitiveTopology(d3d12primitiveTopology);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindViewport(const core::uint4 & _viewport)
    {
        D3D12_VIEWPORT d3d12viewport;
                       d3d12viewport.TopLeftX = (float)_viewport.x;
                       d3d12viewport.TopLeftY = (float)_viewport.y;
                       d3d12viewport.Width = (float)_viewport.z;
                       d3d12viewport.Height = (float)_viewport.w;
                       d3d12viewport.MinDepth = 0.0f;
                       d3d12viewport.MaxDepth = 1.0f;

        m_d3d12graphicsCmdList->RSSetViewports(1, &d3d12viewport);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindScissor(const core::uint4 & _scissor)
    {
        D3D12_RECT d3d12scissor;
                   d3d12scissor.left = _scissor.x;
                   d3d12scissor.top = _scissor.y;
                   d3d12scissor.right = _scissor.z;
                   d3d12scissor.bottom = _scissor.w;

        m_d3d12graphicsCmdList->RSSetScissorRects(1, &d3d12scissor);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindGraphicRootConstants(core::uint(&_constants)[max_root_constants])
    {
        const auto & rootConstantDesc = m_graphicStateCache.m_rootSignature->getRootSignatureDesc().getRootConstants();
        for (uint i = 0; i < rootConstantDesc.size(); ++i)
        {
            const RootSignatureDesc::PushConstantParams & param = rootConstantDesc[i];
            m_d3d12graphicsCmdList->SetGraphicsRoot32BitConstants(i, param.m_count, &_constants[param.m_register], 0);
        }
    }

	//--------------------------------------------------------------------------------------
	void CommandList::clear(const core::float4 & _color)
	{
		//const gfx::RenderPass * renderPass = getRenderPass();
		//const uint subPassIndex = getSubPassIndex();
		//
		//const auto & subPasses = renderPass->getSubPasses();
		//const UserPass * subPass = subPasses[subPassIndex];
		//
		//const auto & renderTargets = subPass->getRenderTargets();
		//for (const auto & renderTarget : renderTargets)
		//{
		//	const gfx::Texture * tex = renderTarget->getTexture();
		//
		//	const auto handle = tex->getd3d12RTVHandle();
		//
		//	const FLOAT clearColor[] =
		//	{
		//		_color.r, _color.g, _color.b, _color.a
		//	};
		//
		//	m_d3d12graphicsCmdList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
		//}
	}

    //--------------------------------------------------------------------------------------
    void CommandList::draw(core::uint _vertexCount, core::uint _startOffset)
    {
        m_d3d12graphicsCmdList->DrawInstanced(_vertexCount, 1, _startOffset, 0);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::drawIndexed(core::uint _indexCount, core::uint _startIndex, core::uint _baseVertex)
    {
        m_d3d12graphicsCmdList->DrawIndexedInstanced(_indexCount, 1, _startIndex, _baseVertex, 0);
    }

    //--------------------------------------------------------------------------------------
    D3D12_PRIMITIVE_TOPOLOGY CommandList::getd3d12PrimitiveTopology(PrimitiveTopology _topology)
    {
        switch (_topology)
        {
        case PrimitiveTopology::PointList:
            return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        case PrimitiveTopology::LineList:
            return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        default:
            VG_ASSERT(false, "Unhandled PrimitiveTopology \"%s\" (%u)", asString(_topology).c_str(), _topology);
        case PrimitiveTopology::TriangleList:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case PrimitiveTopology::LineStrip:
            return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case PrimitiveTopology::TriangleStrip:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        }
    }

    //--------------------------------------------------------------------------------------
    void * CommandList::map(gfx::Buffer * _buffer)
    {       
        auto * device = gfx::Device::get();
        auto uploadBuffer = device->getUploadBuffer();
        return uploadBuffer->map(_buffer->getBufDesc().size(), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
    }
    
    //--------------------------------------------------------------------------------------
    void CommandList::unmap(gfx::Buffer * _buffer, void * _data)
    {
        auto * device = gfx::Device::get();
        auto uploadBuffer = device->getUploadBuffer();
        uploadBuffer->unmap(_buffer, (u8 *)_data);
        uploadBuffer->flush((gfx::CommandList*)this);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::copyBuffer(gfx::Buffer * _dst, gfx::Buffer * _src, core::uint_ptr _srcOffset)
    {
        auto * device = gfx::Device::get();
        auto & context = device->getCurrentFrameContext();

        const auto & bufDesc = _dst->getBufDesc();

        ID3D12Resource * dst = _dst->getResource().getd3d12BufferResource();
        ID3D12Resource * src = _src->getResource().getd3d12BufferResource();

        // Generic read to copy dest barrier
        {
            D3D12_RESOURCE_BARRIER barrier;
            barrier.Transition.pResource = _dst->getResource().getd3d12BufferResource();
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            m_d3d12graphicsCmdList->ResourceBarrier(1, &barrier);
        }

        m_d3d12graphicsCmdList->CopyBufferRegion(dst, 0, src, _srcOffset, bufDesc.size());

        // Copy dest to generic read barrier
        {
            D3D12_RESOURCE_BARRIER barrier;
            barrier.Transition.pResource = _dst->getResource().getd3d12BufferResource();
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            m_d3d12graphicsCmdList->ResourceBarrier(1, &barrier);
        }

        if (asBool(BindFlags::ShaderResource & bufDesc.resource.m_bindFlags))
        {
            auto * bindlessTable = device->getBindlessTable();
            bindlessTable->updated3d12descriptor(_dst->getBufferHandle());
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::copyTexture(gfx::Texture * _dst, gfx::Buffer * _src, core::uint_ptr _srcOffset)
    {
        auto * device = gfx::Device::get();
        auto & context = device->getCurrentFrameContext();

        const TextureDesc & texDesc = _dst->getTexDesc();
        const auto fmtSize = Texture::getPixelFormatSize(texDesc.format);

        for (uint i = 0; i < texDesc.mipmaps; ++i)
        {
            D3D12_SUBRESOURCE_FOOTPRINT pitchedDesc = {};
                                        pitchedDesc.Format = Texture::getd3d12PixelFormat(texDesc.format);
                                        pitchedDesc.Width = texDesc.width>>i;
                                        pitchedDesc.Height = texDesc.height>>i;
                                        pitchedDesc.Depth = 1;
                                        pitchedDesc.RowPitch = (uint)alignUp((uint)(pitchedDesc.Width * fmtSize), (uint)D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

            D3D12_TEXTURE_COPY_LOCATION dst = {};
                                        dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                                        dst.SubresourceIndex = i;
                                        dst.pResource = _dst->getResource().getd3d12TextureResource();

            D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTexture2D = {};
                                               placedTexture2D.Offset = _srcOffset + _dst->getSubResourceData(i).offset;
                                               placedTexture2D.Footprint = pitchedDesc; 

            D3D12_TEXTURE_COPY_LOCATION src = {};
                                        src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                                        src.PlacedFootprint = placedTexture2D;
                                        src.pResource = device->getUploadBuffer()->getBuffer()->getResource().getd3d12BufferResource();

                                    
            m_d3d12graphicsCmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
        }

        D3D12_RESOURCE_BARRIER barrier;
        barrier.Transition.pResource = _dst->getResource().getd3d12TextureResource();
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        m_d3d12graphicsCmdList->ResourceBarrier(1, &barrier);

        if (asBool(BindFlags::ShaderResource & texDesc.resource.m_bindFlags))
        {
            auto * bindlessTable = device->getBindlessTable();
            bindlessTable->updated3d12descriptor(_dst->getTextureHandle());
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindComputeRootSignature(gfx::RootSignature * _rootSig)
    {
        auto * d3d12rs = _rootSig->getd3d12RootSignature();
        m_d3d12graphicsCmdList->SetComputeRootSignature(d3d12rs);
        m_d3d12graphicsCmdList->SetComputeRootDescriptorTable(1, gfx::Device::get()->getBindlessTable()->getd3d12GPUDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindComputePipelineState(gfx::ComputePipelineState * _pso)
    {
        auto * d3s12pso = _pso->getd3d12ComputePipelineState();
        m_d3d12graphicsCmdList->SetPipelineState(d3s12pso);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindComputeRootConstants(core::uint(&_constants)[max_root_constants])
    {
        const auto & rootConstantDesc = m_computeStateCache.m_rootSignature->getRootSignatureDesc().getRootConstants();
        for (uint i = 0; i < rootConstantDesc.size(); ++i)
        {
            const RootSignatureDesc::PushConstantParams & param = rootConstantDesc[i];
            m_d3d12graphicsCmdList->SetComputeRoot32BitConstants(i, param.m_count, &_constants[param.m_register], 0);
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::dispatch(core::uint3 _threadGroupCount)
    {
        m_d3d12graphicsCmdList->Dispatch(_threadGroupCount.x, _threadGroupCount.y, _threadGroupCount.z);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::beginGPUEvent(const char * _name, core::u32 _color)
    {
        #ifdef VG_ENABLE_GPU_MARKER
        PIXBeginEvent(getd3d12GraphicsCommandList(), _color, _name);
        #endif
    }

    //--------------------------------------------------------------------------------------
    void CommandList::endGPUEvent()
    {
        #ifdef VG_ENABLE_GPU_MARKER
        PIXEndEvent(getd3d12GraphicsCommandList());
        #endif
    }
}