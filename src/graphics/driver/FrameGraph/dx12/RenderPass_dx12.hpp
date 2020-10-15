namespace vg::graphics::driver::dx12
{
	//--------------------------------------------------------------------------------------
	RenderPass::RenderPass(const RenderPassKey & _key) :
        super::RenderPass(_key)
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
            driver::SubPass * subPass = subPasses[i];

            const SubPassKey & subPassKey = m_renderPassKey.m_subPassKeys[i];

            for (uint i = 0; i < m_attachments.size(); ++i)
            {
                const SubPassKey::AttachmentInfo & info = subPassKey.getColorAttachmentInfo(i);
                if (asBool(SubPassKey::AttachmentFlags::RenderTarget & info.flags))
                {
                    const FrameGraph::TextureResource * res = subPass->getUserPasses()[0]->getRenderTargets()[subPass->m_renderTargetCount]; // Assume subPass attachment order is the same as renderPass order and that we create different subPasses when attachment changes
                    const FrameGraph::TextureResourceDesc & resourceDesc = res->getTextureResourceDesc();

                    auto & renderTargetDesc = subPass->m_d3d12renderPassRenderTargetDesc[i];

                    // will have to set when 'beginPass'
                    //const Texture * tex = res->getTexture();
                    //renderTargetDesc.cpuDescriptor = tex->getd3d12RTVHandle();

                    if (1)
                    {
                        if (asBool(SubPassKey::AttachmentFlags::Clear & info.flags))
                            renderTargetDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
                        else if (asBool(SubPassKey::AttachmentFlags::Preserve & info.flags))
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
		}		
	}
}