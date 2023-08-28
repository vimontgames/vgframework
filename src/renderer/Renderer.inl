namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    gfx::Texture * Renderer::getDefaultTexture(MaterialTextureType _type) const
    {
        return m_defaultTextures[core::asInteger(_type)];
    }

    //--------------------------------------------------------------------------------------
    MaterialModel * Renderer::getDefaultMaterial() const
    {
        return m_defaultMaterial;
    }
}