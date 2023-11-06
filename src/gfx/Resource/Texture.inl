namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        VG_INLINE const TextureDesc & Texture::getTexDesc() const
        { 
            return m_texDesc; 
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE const gfx::Resource & Texture::getResource() const
        { 
            return m_resource; 
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE gfx::Resource & Texture::getResource()
        { 
            return m_resource;
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE const gfx::BindlessTextureHandle Texture::getTextureHandle() const
        {
            VG_ASSERT(m_textureHandle.isValid());
            return m_textureHandle; 
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE const gfx::BindlessRWTextureHandle Texture::getRWTextureHandle() const
        { 
            VG_ASSERT(m_rwTextureHandle.isValid());
            return m_rwTextureHandle; 
        }
    }
}