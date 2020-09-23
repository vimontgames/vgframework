namespace vg::graphics::driver::dx12
{
	//--------------------------------------------------------------------------------------
	CommandList::CommandList(driver::CommandListType _type, driver::CommandPool * _cmdPool, core::uint _frame, core::uint _index) :
		super::CommandList(_type, _cmdPool, _frame, _index)
	{
		Device * device = driver::Device::get();
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
				m_d3d12graphicsCmdList->Reset(getCommandPool()->getd3d12CommandAllocator(), nullptr);
				break;

			default:
				VG_ASSERT_NOT_IMPLEMENTED();
				break;
		}

        m_d3d12graphicsCmdList->SetDescriptorHeaps(1, &driver::Device::get()->m_srvGPUDescriptorHeap);
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
	void CommandList::beginRenderPass(const driver::RenderPass * _renderPass)
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
	void CommandList::beginSubPass(core::uint _subPassIndex, const driver::SubPass * _subPass)
	{
		super::beginSubPass(_subPassIndex, _subPass);

		if (_subPass)
			m_d3d12graphicsCmdList->BeginRenderPass(_subPass->m_renderTargetCount, _subPass->m_renderTargetCount ? _subPass->m_d3d12renderPassRenderTargetDesc : nullptr, _subPass->m_depthStencilCount? &_subPass->m_d3d12renderPassDepthStencilDesc : nullptr, _subPass->m_d3d12renderPassFlags);
	}

	//--------------------------------------------------------------------------------------
	void CommandList::endSubPass()
	{
		m_d3d12graphicsCmdList->EndRenderPass();

		super::endSubPass();
	}

    //--------------------------------------------------------------------------------------
    void CommandList::bindRootSignature(driver::RootSignature * _rootSig)
    {
        auto * d3d12rs = _rootSig->getd3d12RootSignature();
        m_d3d12graphicsCmdList->SetGraphicsRootSignature(d3d12rs);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindGraphicPipelineState(driver::GraphicPipelineState * _pso)
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
    void CommandList::bindRootConstants(core::uint(&_constants)[max_root_constants])
    {
        const auto & rootConstantDesc = m_currentRootSignature->getRootSignatureDesc().getRootConstants();
        for (uint i = 0; i < rootConstantDesc.size(); ++i)
        {
            const RootSignatureDesc::PushConstantParams & param = rootConstantDesc[i];
            m_d3d12graphicsCmdList->SetGraphicsRoot32BitConstants(i, param.m_count, &_constants[param.m_start], 0);
        }
    }

	//--------------------------------------------------------------------------------------
	void CommandList::clear(const core::float4 & _color)
	{
		//const driver::RenderPass * renderPass = getRenderPass();
		//const uint subPassIndex = getSubPassIndex();
		//
		//const auto & subPasses = renderPass->getSubPasses();
		//const UserPass * subPass = subPasses[subPassIndex];
		//
		//const auto & renderTargets = subPass->getRenderTargets();
		//for (const auto & renderTarget : renderTargets)
		//{
		//	const driver::Texture * tex = renderTarget->getTexture();
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
        // hak
        m_d3d12graphicsCmdList->SetGraphicsRootDescriptorTable(1, driver::Device::get()->m_srvGPUDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

        m_d3d12graphicsCmdList->DrawInstanced(_vertexCount, 1, _startOffset, 0);
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
            VG_ASSERT(false, "Unhandled PrimitiveTopology \"%s\" (%u)", asString(_topology), _topology);
        case PrimitiveTopology::TriangleList:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case PrimitiveTopology::LineStrip:
            return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case PrimitiveTopology::TriangleStrip:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::copyTexture(driver::Texture * _dst, core::u32 _from)
    {
        auto * device = driver::Device::get();
        auto & context = device->getCurrentFrameContext();

        const auto & texDesc = _dst->getTexDesc();
        const auto fmtSize = Texture::getPixelFormatSize(texDesc.format);

        D3D12_SUBRESOURCE_FOOTPRINT pitchedDesc = {};
                                    pitchedDesc.Format = Texture::getd3d12PixelFormat(texDesc.format);
                                    pitchedDesc.Width = texDesc.width;
                                    pitchedDesc.Height = texDesc.height;
                                    pitchedDesc.Depth = 1;
                                    pitchedDesc.RowPitch = (uint)alignUp(texDesc.width * fmtSize, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

        D3D12_TEXTURE_COPY_LOCATION dst = {};
                                    dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                                    dst.SubresourceIndex = 0;
                                    dst.pResource = _dst->getResource().getd3d12TextureResource();

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTexture2D = {};
                                           placedTexture2D.Offset = _from;
                                           placedTexture2D.Footprint = pitchedDesc;

        D3D12_TEXTURE_COPY_LOCATION src = {};
                                    src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                                    src.PlacedFootprint = placedTexture2D;
                                    src.pResource = context.m_uploadBuffer->getResource().getd3d12BufferResource();

        m_d3d12graphicsCmdList->CopyTextureRegion( &dst, 0, 0, 0, &src, nullptr);
    }
}