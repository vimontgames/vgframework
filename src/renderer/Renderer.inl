namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::Texture * Renderer::getDefaultTexture(MaterialTextureType _type) const
    {
        return m_defaultTextures[core::asInteger(_type)];
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE MaterialModel * Renderer::getDefaultMaterial() const
    {
        return m_defaultMaterial;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE ImGuiAdapter * Renderer::getImGuiAdapter() const
    {
        return m_imgui; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE SharedWorldCullingJobOutput * Renderer::getSharedWorldCullingJobOutput() const
    {
        return m_sharedWorldCullingJobOutput;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE SharedViewCullingJobOutput * Renderer::getSharedCullingJobOutput() const
    {
        return m_sharedViewCullingJobOutput; 
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE MaterialManager * Renderer::getMaterialManager() const
    {
        return m_materialManager;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE ParticleManager * Renderer::getParticleManager() const
    {
        return m_particleManager;
    }
}