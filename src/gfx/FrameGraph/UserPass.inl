namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const core::vector<FrameGraphTextureResource *> & UserPass::getRenderTargets() const
    {
        return m_renderTargets;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE FrameGraphTextureResource * UserPass::getDepthStencil() const
    {
        return m_depthStencil;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::vector<FrameGraphTextureResource *> & UserPass::getRWTextures() const
    {
        return m_rwTextures;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::vector<FrameGraphBufferResource *> & UserPass::getRWBuffers() const
    {
        return m_rwBuffers;
    }
}