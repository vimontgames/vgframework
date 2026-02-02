namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::Texture * Renderer::getDefaultTexture(DefaultTextureType _type) const
    {
        VG_ASSERT(asInteger(_type) < countof(m_defaultTextures));
        return m_defaultTextures[core::asInteger(_type)];
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE MaterialModel * Renderer::getDefaultMaterial(DefaultMaterialType _type) const
    {
        VG_ASSERT(asInteger(_type) < countof(m_defaultMaterials));
        return m_defaultMaterials[asInteger(_type)];
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