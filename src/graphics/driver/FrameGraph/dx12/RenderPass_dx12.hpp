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
                const auto flags = subPassKey.getRenderTargetFlags(i);
                if (SubPassKey::Flags::Bind & flags)
                {
                    const FrameGraph::TextureResource * res = subPass->getUserPasses()[0]->getRenderTargets()[subPass->m_renderTargetCount]; // Assume subPass attachment order is the same as renderPass order and that we create different subPasses when attachment changes
                    const Texture * tex = res->getTexture();
                    const FrameGraph::TextureDesc & resourceDesc = res->getTextureDesc();

                    auto & renderTargetDesc = subPass->m_d3d12renderPassRenderTargetDesc[i];

                    renderTargetDesc.cpuDescriptor = tex->getd3d12RTVHandle();

                    if (1)
                    {
                        switch (resourceDesc.initState)
                        {
                            default:
                                VG_ASSERT(false, "Unhandled FrameGraph::Resource::InitState \"%s\" (%u)", asString(resourceDesc.initState), resourceDesc.initState);

                            case FrameGraph::Resource::InitState::Discard:
                                renderTargetDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
                                break;

                            case FrameGraph::Resource::InitState::Clear:
                                renderTargetDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
                                break;

                            case FrameGraph::Resource::InitState::Preserve:
                                renderTargetDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
                                break;
                        }
                        
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