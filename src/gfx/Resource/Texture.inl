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
            VG_ASSERT(!isDepthStencilFormat(getTexDesc().format), "[Device] Texture \"%s\" is a depthstencil texture, you should use 'getDepthTextureHandle()' to retrieve its bindless handle", GetName().c_str());
            VG_ASSERT(m_textureHandle.isValid());
            return m_textureHandle; 
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE const gfx::BindlessTextureHandle Texture::getDepthTextureHandle() const
        {
            VG_ASSERT(isDepthStencilFormat(getTexDesc().format), "[Device] Texture \"%s\" is not a depth texture, you should use 'getTextureHandle()' to retrieve its bindless handle", GetName().c_str());
            VG_ASSERT(m_textureHandle.isValid());
            return m_depthTextureHandle;
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE const gfx::BindlessTextureHandle Texture::getStencilTextureHandle() const
        {
            VG_ASSERT(isDepthStencilFormat(getTexDesc().format), "[Device] Texture \"%s\" is not a stencil texture, you should use 'getTextureHandle()' to retrieve its bindless handle", GetName().c_str());
            VG_ASSERT(m_textureHandle.isValid());
            return m_stencilTextureHandle;
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE const gfx::BindlessRWTextureHandle Texture::getRWTextureHandle(uint _mipLevel) const
        { 
            VG_ASSERT(_mipLevel < m_rwTextureMipHandles.size());
            const auto & rwTextureHandle = m_rwTextureMipHandles[_mipLevel];
            VG_ASSERT(rwTextureHandle.isValid());
            return rwTextureHandle;
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE bool Texture::isDepthStencilFormat(PixelFormat _format)
        {
            switch (_format)
            {
                default:
                    VG_ASSERT(false, "Unhandled PixelFormat \"%s\" (%u)", core::asString(_format).c_str(), _format);
                case PixelFormat::R8G8B8A8_unorm:
                case PixelFormat::R8G8B8A8_unorm_sRGB:
                case PixelFormat::B8G8R8A8_unorm:
                case PixelFormat::B8G8R8A8_unorm_sRGB:
                case PixelFormat::R10G10B10A2_unorm:
                case PixelFormat::R16G16B16A16_unorm:
                case PixelFormat::R16G16B16A16_float:
                case PixelFormat::R16G16B16A16_uint:
                case PixelFormat::R16G16_float:
                case PixelFormat::R32G32B32A32_float:
                case PixelFormat::R32_uint:  
                case PixelFormat::R32G32_uint:
                    return false;

                case PixelFormat::D32S8:
                    return true;
            }
        }
    }
}