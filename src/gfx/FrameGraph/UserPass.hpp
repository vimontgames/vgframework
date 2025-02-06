#include "gfx/Precomp.h"
#include "UserPass.h"

#if !VG_ENABLE_INLINE
#include "UserPass.inl"
#endif

using namespace vg::core;

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    static string makeUniqueName(const string & _name, const RenderPassContext & _renderContext)
    {
        return _name + "-" + _renderContext.getView()->GetName();
    }

	//--------------------------------------------------------------------------------------
	UserPass::UserPass(const core::string & _name)
	{
        SetName(_name);
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
        m_rwBuffers.clear();
	}

	//--------------------------------------------------------------------------------------
	void UserPass::setFrameGraph(FrameGraph * _frameGraph)
	{
		m_frameGraph = _frameGraph;
	}

    //--------------------------------------------------------------------------------------
    const FrameGraphTextureResourceDesc * UserPass::getTextureResourceDesc(const FrameGraphResourceID & _resID) const
    {
        if (const auto * res = m_frameGraph->getTextureResource(_resID))
            return &res->getTextureResourceDesc();
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    const FrameGraphBufferResourceDesc * UserPass::getBufferResourceDesc(const FrameGraphResourceID & _resID) const
    {
        if (const auto * res = m_frameGraph->getBufferResource(_resID))
            return &res->getBufferResourceDesc();
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    // If resource already exist, it must have exactly the same descriptor
    //--------------------------------------------------------------------------------------
    FrameGraphTextureResource * UserPass::createTexture(const FrameGraphResourceID & _resID, FrameGraphTextureResourceDesc & _resDesc)
    {
        _resDesc.transient = true;
        FrameGraphTextureResource * res = m_frameGraph->addTextureResource(_resID, _resDesc);
        VG_ASSERT(res);
        return res;
    }

    //--------------------------------------------------------------------------------------
    void UserPass::createRenderTarget(const FrameGraphResourceID & _resID, FrameGraphTextureResourceDesc & _resDesc)
    {
        _resDesc.renderTarget = true;
        FrameGraphTextureResource * res = createTexture(_resID, _resDesc);
        res->setCurrentState(ResourceState::RenderTarget);
    }

    //--------------------------------------------------------------------------------------
    void UserPass::createDepthStencil(const FrameGraphResourceID & _resID, FrameGraphTextureResourceDesc & _resDesc)
    {
        FrameGraphTextureResource * res = createTexture(_resID, _resDesc);
        res->setCurrentState(ResourceState::RenderTarget);
    }

    //--------------------------------------------------------------------------------------
    void UserPass::createRWTexture(const FrameGraphResourceID & _resID, FrameGraphTextureResourceDesc & _resDesc)
    {
        _resDesc.uav = true;
        FrameGraphTextureResource * res = createTexture(_resID, _resDesc);
        res->setCurrentState(ResourceState::UnorderedAccess);
    }

    //--------------------------------------------------------------------------------------
    FrameGraphBufferResource * UserPass::createBuffer(const FrameGraphResourceID & _resID, FrameGraphBufferResourceDesc & _resDesc)
    {
        _resDesc.transient = true;
        FrameGraphBufferResource * res = m_frameGraph->addBufferResource(_resID, _resDesc);
        VG_ASSERT(res);
        return res;
    }

    //--------------------------------------------------------------------------------------
    void UserPass::createRWBuffer(const FrameGraphResourceID & _resID, FrameGraphBufferResourceDesc & _resDesc)
    {
        _resDesc.uav = true;
        FrameGraphBufferResource * res = createBuffer(_resID, _resDesc);
        res->setCurrentState(ResourceState::UnorderedAccess);
    }
    
	//--------------------------------------------------------------------------------------
    // Resource should be:
    // - Persistent and already imported in the graph
    // - Transient and already declared in the graph
    //--------------------------------------------------------------------------------------
	void UserPass::writeRenderTarget(uint _slot, const FrameGraphResourceID & _resID)
	{
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        res->setWriteAtPass(this);
        VG_ASSERT(m_renderTargets.size() == _slot);
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
    void UserPass::writeRWTexture(const FrameGraphResourceID & _resID)
    {
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res);
        res->setWriteAtPass(this);
        m_rwTextures.push_back(res);
    }

    //--------------------------------------------------------------------------------------
    void UserPass::writeRWBuffer(const FrameGraphResourceID & _resID)
    {
        FrameGraphBufferResource * res = m_frameGraph->getBufferResource(_resID);
        VG_ASSERT(res);
        res->setWriteAtPass(this);
        m_rwBuffers.push_back(res);
    }

    //--------------------------------------------------------------------------------------
    void UserPass::readRWBuffer(const FrameGraphResourceID & _resID)
    {
        FrameGraphBufferResource * res = m_frameGraph->getBufferResource(_resID);
        VG_ASSERT(res);
        res->setReadAtPass(this);
        m_rwBuffers.push_back(res);
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
    void UserPass::readDepthStencil(const core::vector<FrameGraphResourceID> & _resIDs)
    {
        for (auto & id : _resIDs)
            readDepthStencil(id);
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
    // Get RenderTarget '_resID' during Render
    //--------------------------------------------------------------------------------------
    Texture * UserPass::getRenderTarget(const FrameGraphResourceID & _resID) const
    {
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res, "RenderTarget \"%s\" does not exist in FrameGraph", _resID.c_str());
        VG_ASSERT(res->getTexture(), "RenderTarget \"%s\" exists in FrameGraph but is not allocated", _resID.c_str());
        return res->getTexture();
    }

    //--------------------------------------------------------------------------------------
    // Get DepthStencil '_resID' during Render
    //--------------------------------------------------------------------------------------
    Texture * UserPass::getDepthStencil(const FrameGraphResourceID & _resID) const
    {
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res, "RenderTarget \"%s\" does not exist in FrameGraph", _resID.c_str());
        VG_ASSERT(res->getTexture(), "DepthStencil \"%s\" exists in FrameGraph but is not allocated", _resID.c_str());
        return res->getTexture();
    }

    //--------------------------------------------------------------------------------------
    // Get RWTexture '_resID'during Render
    //--------------------------------------------------------------------------------------
    Texture * UserPass::getRWTexture(const FrameGraphResourceID & _resID) const
    {
        FrameGraphTextureResource * res = m_frameGraph->getTextureResource(_resID);
        VG_ASSERT(res, "RWTexture \"%s\" does not exist in FrameGraph", _resID.c_str());
        VG_ASSERT(res->getTexture(), "RWTexture \"%s\" exists in FrameGraph but is not allocated", _resID.c_str());
        return res->getTexture();
    }

    //--------------------------------------------------------------------------------------
    // Get getRWBuffer '_resID' during Render
    //--------------------------------------------------------------------------------------
    Buffer * UserPass::getRWBuffer(const FrameGraphResourceID & _resID) const
    {
        FrameGraphBufferResource * res = m_frameGraph->getBufferResource(_resID);
        VG_ASSERT(res,"RWBuffer \"%s\" does not exist in FrameGraph", _resID.c_str());
        VG_ASSERT(res->getBuffer(), "RWBuffer \"%s\" exists in FrameGraph but is not allocated", _resID.c_str());
        return res->getBuffer();
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