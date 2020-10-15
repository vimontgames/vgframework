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
        m_textures.clear();
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
    // If resource already exist, it must have exactly the same descriptor
    //--------------------------------------------------------------------------------------
    void UserPass::createRenderTarget(const FrameGraph::ResourceID & _resID, FrameGraph::TextureResourceDesc & _resDesc)
    {
        _resDesc.transient = true;

        FrameGraph::TextureResource * res = m_frameGraph->addTextureResource(_resID, _resDesc);
        VG_ASSERT(res);
    }

	//--------------------------------------------------------------------------------------
    // Resource should be:
    // - Persistent and already imported in the graph
    // - Transient and already declared in the graph
    //--------------------------------------------------------------------------------------
	void UserPass::writeRenderTarget(core::uint _slot, const FrameGraph::ResourceID & _resID)
	{
        FrameGraph::TextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        res->setWriteAtPass(this);
		m_renderTarget.push_back(res);
	}

    //--------------------------------------------------------------------------------------
    // The pass will read RT '_resID'
    //--------------------------------------------------------------------------------------
    void UserPass::readRenderTarget(const FrameGraph::ResourceID & _resID)
    {
        FrameGraph::TextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        res->setReadAtPass(this);
        m_textures.push_back(res);
    }

    //--------------------------------------------------------------------------------------
    // Get RenderTarget '_resID' for read during 'render'
    //--------------------------------------------------------------------------------------
    Texture * UserPass::getRenderTarget(const FrameGraph::ResourceID & _resID) const
    {
        FrameGraph::TextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        return res->getTexture();
    }

    //--------------------------------------------------------------------------------------
    core::uint UserPass::getRenderTargetCount() const
    {
        return (core::uint)m_renderTarget.size();
    }

    //--------------------------------------------------------------------------------------
    const FrameGraph::TextureResourceDesc & UserPass::getRenderTargetDesc(core::uint _index) const
    {
        return m_renderTarget[_index]->getTextureResourceDesc();
    }

    //--------------------------------------------------------------------------------------
    bool UserPass::hasDepthStencil() const
    {
        return nullptr != m_depthStencil;
    }

    //--------------------------------------------------------------------------------------
    const FrameGraph::TextureResourceDesc & UserPass::getDepthStencilDesc() const
    {
        return m_depthStencil->getTextureResourceDesc();
    }
}