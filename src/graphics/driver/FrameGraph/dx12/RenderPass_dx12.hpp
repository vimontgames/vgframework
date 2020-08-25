namespace vg::graphics::driver::dx12
{
	//--------------------------------------------------------------------------------------
	RenderPass::RenderPass()
	{

	}

	//--------------------------------------------------------------------------------------
	RenderPass::~RenderPass()
	{

	}

	//--------------------------------------------------------------------------------------
	void RenderPass::finalize()
	{
		for (driver::SubPass * subPass : getSubPasses())
		{
			const auto & userPass = subPass->m_userPasses[0];
			const auto & renderTargets = userPass->getRenderTargets();

			for (uint i = 0; i < renderTargets.size(); ++i)
			{
				const FrameGraph::TextureResource * res = renderTargets[i];
				const Texture * tex = res->getTexture();
				const FrameGraph::TextureDesc & resourceDesc = res->getTextureDesc();

				auto & renderTargetDesc = subPass->m_d3d12renderPassRenderTargetDesc[i];

				renderTargetDesc.cpuDescriptor = tex->getd3d12RTVHandle();
		
				if (1)
				{
					renderTargetDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
					renderTargetDesc.BeginningAccess.Clear.ClearValue.Color[0] = resourceDesc.clearColor.r;
					renderTargetDesc.BeginningAccess.Clear.ClearValue.Color[1] = resourceDesc.clearColor.g;
					renderTargetDesc.BeginningAccess.Clear.ClearValue.Color[2] = resourceDesc.clearColor.b;
					renderTargetDesc.BeginningAccess.Clear.ClearValue.Color[3] = resourceDesc.clearColor.a;
				}
		
				renderTargetDesc.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
			}

			subPass->m_renderTargetCount++;
		}		
	}
}