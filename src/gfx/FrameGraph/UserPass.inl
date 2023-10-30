namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const core::vector<FrameGraph::TextureResource *> & UserPass::getRenderTargets() const
    {
        return m_renderTarget;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::vector<FrameGraph::TextureResource *> & UserPass::getRWTextures() const
    {
        return m_readWriteTexture;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE FrameGraph::TextureResource * UserPass::getDepthStencil() const
    {
        return m_depthStencil;
    }
}