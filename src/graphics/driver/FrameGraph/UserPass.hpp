#include "graphics/driver/Precomp.h"
#include "UserPass.h"

#include VG_GFXAPI_IMPL(SubPass)

using namespace vg::core;

namespace vg::graphics::driver
{
	//--------------------------------------------------------------------------------------
	UserPass::UserPass()
	{
		reset();
	}

	//--------------------------------------------------------------------------------------
	UserPass::~UserPass()
	{

	}

	//--------------------------------------------------------------------------------------
	void UserPass::reset()
	{
		m_depthStencil = nullptr;
		m_renderTarget.clear();
	}

	//--------------------------------------------------------------------------------------
	void UserPass::setFrameGraph(FrameGraph * _frameGraph)
	{
		m_frameGraph = _frameGraph;
	}

	//--------------------------------------------------------------------------------------
	const core::vector<FrameGraph::TextureResource *> & UserPass::getRenderTargets() const
	{
		return m_renderTarget;
	}

    //--------------------------------------------------------------------------------------
    FrameGraph::TextureResource * UserPass::getDepthStencil() const
    {
        return m_depthStencil;
    }

	//--------------------------------------------------------------------------------------
	void UserPass::writeRenderTarget(core::uint _slot, const FrameGraph::ResourceID & _resID, const FrameGraph::TextureDesc & _resDesc)
	{
		FrameGraph::TextureResource & res = m_frameGraph->addTextureResource(_resID);

		res.setWriteAtPass(this);
		res.setTextureDesc(_resDesc);
		res.setTextureUsage(FrameGraph::TextureResource::Usage::RenderTarget);

		m_renderTarget.push_back(&res);
	}

    //--------------------------------------------------------------------------------------
    core::uint UserPass::getRenderTargetCount() const
    {
        return (core::uint)m_renderTarget.size();
    }

    //--------------------------------------------------------------------------------------
    const FrameGraph::TextureDesc & UserPass::getRenderTargetDesc(core::uint _index) const
    {
        return m_renderTarget[_index]->getTextureDesc();
    }

    //--------------------------------------------------------------------------------------
    bool UserPass::hasDepthStencil() const
    {
        return nullptr != m_depthStencil;
    }

    //--------------------------------------------------------------------------------------
    const FrameGraph::TextureDesc & UserPass::getDepthStencilDesc() const
    {
        return m_depthStencil->getTextureDesc();
    }
}