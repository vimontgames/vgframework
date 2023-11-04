#include "gfx/Precomp.h"
#include "UserPass.h"

#include VG_GFXAPI_IMPL(SubPass)

#if !VG_ENABLE_INLINE
#include "UserPass.inl"
#endif

using namespace vg::core;

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    static string makeUniqueName(const string & _name, const RenderPassContext _renderContext)
    {
        return _name + "-" + _renderContext.m_view->getName();
    }

	//--------------------------------------------------------------------------------------
	UserPass::UserPass(const core::string & _name)
	{
        setName(_name);
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
		m_renderTargets.clear();
        m_rwTextures.clear();
	}

	//--------------------------------------------------------------------------------------
	void UserPass::setFrameGraph(FrameGraph * _frameGraph)
	{
		m_frameGraph = _frameGraph;
	}

    //--------------------------------------------------------------------------------------
    // If resource already exist, it must have exactly the same descriptor
    //--------------------------------------------------------------------------------------
    void UserPass::createRenderTarget(const FrameGraphResourceID & _resID, FrameGraphTextureResourceDesc & _resDesc)
    {
        _resDesc.transient = true;

        FrameGraphTextureResource * res = m_frameGraph->addTextureResource(_resID, _resDesc);
        VG_ASSERT(res);
    }

    //--------------------------------------------------------------------------------------
    void UserPass::createDepthStencil(const FrameGraphResourceID & _resID, FrameGraphTextureResourceDesc & _resDesc)
    {
        createRenderTarget(_resID, _resDesc);
    }

    //--------------------------------------------------------------------------------------
    void UserPass::createRWTexture(const FrameGraphResourceID & _resID, FrameGraphTextureResourceDesc & _resDesc)
    {
        _resDesc.transient = true;
        _resDesc.uav = true;

        createRenderTarget(_resID, _resDesc);
    }

	//--------------------------------------------------------------------------------------
    // Resource should be:
    // - Persistent and already imported in the graph
    // - Transient and already declared in the graph
    //--------------------------------------------------------------------------------------
	void UserPass::writeRenderTarget(core::uint _slot, const FrameGraphResourceID & _resID)
	{
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        res->setWriteAtPass(this);
		m_renderTargets.push_back(res);
	}

    //--------------------------------------------------------------------------------------
    void UserPass::writeDepthStencil(const FrameGraphResourceID & _resID)
    {
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        res->setWriteAtPass(this);
        m_depthStencil = res;
    }

    //--------------------------------------------------------------------------------------
    void UserPass::writeRWTexture(core::uint _slot, const FrameGraphResourceID & _resID)
    {
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        res->setWriteAtPass(this);
        m_rwTextures.push_back(res);
    }

    //--------------------------------------------------------------------------------------
    // The pass will read RT '_resID'
    //--------------------------------------------------------------------------------------
    void UserPass::readRenderTarget(const FrameGraphResourceID & _resID)
    {
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        res->setReadAtPass(this);
        m_textures.push_back(res);
    }

    //--------------------------------------------------------------------------------------
    void UserPass::readDepthStencil(const FrameGraphResourceID & _resID)
    {
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        res->setReadAtPass(this);
        m_textures.push_back(res);
    }

    //--------------------------------------------------------------------------------------
    void UserPass::readRWTexture(const FrameGraphResourceID & _resID)
    {
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        res->setReadAtPass(this);
        m_rwTextures.push_back(res);
    }

    //--------------------------------------------------------------------------------------
    // Get RenderTarget '_resID' for read during 'render'
    //--------------------------------------------------------------------------------------
    Texture * UserPass::getRenderTarget(const FrameGraphResourceID & _resID) const
    {
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        return res->getTexture();
    }

    //--------------------------------------------------------------------------------------
    core::uint UserPass::getRenderTargetCount() const
    {
        return (core::uint)m_renderTargets.size();
    }

    //--------------------------------------------------------------------------------------
    const FrameGraphTextureResourceDesc & UserPass::getRenderTargetDesc(core::uint _index) const
    {
        return m_renderTargets[_index]->getTextureResourceDesc();
    }

    //--------------------------------------------------------------------------------------
    bool UserPass::hasDepthStencil() const
    {
        return nullptr != m_depthStencil;
    }

    //--------------------------------------------------------------------------------------
    const FrameGraphTextureResourceDesc & UserPass::getDepthStencilDesc() const
    {
        return m_depthStencil->getTextureResourceDesc();
    }

    //--------------------------------------------------------------------------------------
    void UserPass::setUserPassType(RenderPassType _userPassType)
    {
        VG_ASSERT(isEnumValue(_userPassType));
        m_userPassType = _userPassType;
    }

    //--------------------------------------------------------------------------------------
    RenderPassType UserPass::getUserPassType() const
    {
        return m_userPassType;
    }
}