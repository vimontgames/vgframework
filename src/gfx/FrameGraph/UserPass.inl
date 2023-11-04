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
}