namespace vg::gfx::dx12
{
	//--------------------------------------------------------------------------------------
	RenderPass::RenderPass(RenderPassType _renderPassType, const RenderPassKey & _key) :
        super::RenderPass(_renderPassType, _key)
	{

	}

	//--------------------------------------------------------------------------------------
	RenderPass::~RenderPass()
	{

	}

	//--------------------------------------------------------------------------------------
	void RenderPass::finalize()
	{
        const auto subPasses = getSubPasses();
        const auto subPassCount = subPasses.size();
		for (uint i = 0; i < subPassCount; ++i)
		{
            gfx::SubPass * subPass = subPasses[i];

            const SubPassKey & subPassKey = m_renderPassKey.m_subPassKeys[i];

            for (uint i = 0; i < m_colorAttachments.size(); ++i)
            {
                const ResourceTransitionDesc & info = subPassKey.getColorAttachmentInfo(i);
                if (asBool(ResourceTransitionFlags::RenderTarget & info.flags))
                {
                    const FrameGraphTextureResource * res = subPass->getUserPassesInfos()[0].m_userPass->getRenderTargets()[subPass->m_renderTargetCount]; // Assume subPass attachment order is the same as renderPass order and that we create different subPasses when attachment changes
                    const FrameGraphTextureResourceDesc & resourceDesc = res->getTextureResourceDesc();

                    D3D12_RENDER_PASS_RENDER_TARGET_DESC & renderTargetDesc = subPass->m_d3d12renderPassRenderTargetDesc[i];

                    // will have to set when 'beginPass'
                    //const Texture * tex = res->getTexture();
                    //renderTargetDesc.cpuDescriptor = tex->getd3d12RTVHandle();

                    if (1)
                    {
                        if (asBool(ResourceTransitionFlags::Clear & info.flags))
                            renderTargetDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
                        else if (asBool(ResourceTransitionFlags::Preserve & info.flags))
                            renderTargetDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
                        else
                            renderTargetDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
                            
                        renderTargetDesc.BeginningAccess.Clear.ClearValue.Color[0] = resourceDesc.clearColor.r;
                        renderTargetDesc.BeginningAccess.Clear.ClearValue.Color[1] = resourceDesc.clearColor.g;
                        renderTargetDesc.BeginningAccess.Clear.ClearValue.Color[2] = resourceDesc.clearColor.b;
                        renderTargetDesc.BeginningAccess.Clear.ClearValue.Color[3] = resourceDesc.clearColor.a;
                    }

                    renderTargetDesc.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
                }

                subPass->m_renderTargetCount++;
            }

            if (m_depthStencilAttachment)
            {
                const ResourceTransitionDesc & info = subPassKey.getDepthStencilAttachmentInfo();
                if (asBool(ResourceTransitionFlags::RenderTarget & info.flags))
                {
                    const FrameGraphTextureResource * res = subPass->getUserPassesInfos()[0].m_userPass->getDepthStencil();
                    const FrameGraphTextureResourceDesc & resourceDesc = res->getTextureResourceDesc();

                    D3D12_RENDER_PASS_DEPTH_STENCIL_DESC & depthStencilDesc = subPass->m_d3d12renderPassDepthStencilDesc;

                    if (1)
                    {
                        if (asBool(ResourceTransitionFlags::Clear & info.flags))
                        {
                            depthStencilDesc.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
                            depthStencilDesc.StencilBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
                        }
                        else if (asBool(ResourceTransitionFlags::Preserve & info.flags))
                        {
                            depthStencilDesc.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
                            depthStencilDesc.StencilBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
                        }
                        else
                        {
                            depthStencilDesc.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
                            depthStencilDesc.StencilBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
                        }

                        depthStencilDesc.DepthBeginningAccess.Clear.ClearValue.DepthStencil = { defaultOptimizedClearDepth, defaultOptimizedClearStencil };
                        depthStencilDesc.StencilBeginningAccess.Clear.ClearValue.DepthStencil = { defaultOptimizedClearDepth, defaultOptimizedClearStencil };
                    }

                    depthStencilDesc.DepthEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
                    depthStencilDesc.StencilEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
                }

                subPass->m_depthStencilCount = 1;
            }
		}		
	}
}