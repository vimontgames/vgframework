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
	DirectX12GraphicsCommandList * CommandList::getd3d12GraphicsCommandList()
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
}