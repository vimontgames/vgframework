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
		m_depthStencilIn = nullptr;
		m_depthStencilOut = nullptr;

		m_renderTargetIn.clear();
		m_renderTargetOut.clear();
	}

	//--------------------------------------------------------------------------------------
	void UserPass::setFrameGraph(FrameGraph * _frameGraph)
	{
		m_frameGraph = _frameGraph;
	}

	//--------------------------------------------------------------------------------------
	const core::vector<FrameGraph::TextureResource *> & UserPass::getRenderTargets() const
	{
		return m_renderTargetOut;
	}

	//--------------------------------------------------------------------------------------
	void UserPass::writeRenderTarget(core::uint _slot, const FrameGraph::ResourceID & _resID, const FrameGraph::TextureDesc & _resDesc)
	{
		FrameGraph::TextureResource & res = m_frameGraph->addTextureResource(_resID);

		res.setWriteAtPass(this);
		res.setTextureDesc(_resDesc);
		res.setTextureUsage(FrameGraph::TextureResource::Usage::RenderTarget);

		m_renderTargetIn.push_back(nullptr);
		m_renderTargetOut.push_back(&res);
	}
}