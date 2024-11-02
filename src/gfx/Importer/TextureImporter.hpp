#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) VG_ASSERT(x)
#include "stb/stb_image.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Importer/TextureImporterSettings.h"

using namespace vg::core;

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    bool TextureImporter::importTextureData(const core::string & _path, TextureDesc & _desc, vector<u8> & _buffer, const TextureImporterSettings * _importSettings)
    {
        // TODO: support Texture3D/Array loading?
        int srcWidth, srcHeight, srcChannels;
        stbi_uc * data = stbi_load(_path.c_str(), &srcWidth, &srcHeight, &srcChannels, 4);

        if (nullptr != data)
        {
            TextureImporterType type;
            TextureImporterFormat format;
            bool sRGB;
            core::uint mipmapCount;
            uint width, height, slices;

            if (_importSettings && TextureImporterType::Automatic != _importSettings->m_importerType)
                type = _importSettings->m_importerType;
            else
                type = TextureImporterType::Texture2D; 

            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    //break;

                case TextureImporterType::Texture2D:
                    _desc.type = TextureType::Texture2D;
                    width = srcWidth;
                    height = srcHeight;
                    slices = 1;
                    break;

                case TextureImporterType::Cubemap:
                    _desc.type = TextureType::TextureCube;

                    if (srcWidth * 3 == srcHeight * 4)
                    {
                        // Horizontal cross 
                        // +----+----+----+----+
                        // |    | Z+ |    |    |
                        // +----+----+----+----+
                        // | X- | Y- | X+ | Y+ |
                        // +----+----+----+----+
                        // |    | Z- |    |    |
                        // +----+----+----+----+

                        width = srcWidth / 4;
                        height = srcHeight / 3;
                        slices = 6;
                    }
                    else
                    {
                        VG_ASSERT(false, "Cubemap has invalid dimensions %ux%u", srcWidth, srcHeight);
                        VG_SAFE_FREE(data);
                        return false;
                    }                    

                    // recompute mipmap count
                    if (_importSettings && MipLevelCount::Automatic != _importSettings->m_mipLevelCount)
                    {
                        const uint maxMipCount = computeMaxMipmapCount(type, width, height, slices);
                        mipmapCount = min((uint)_importSettings->m_mipLevelCount, maxMipCount);
                    }
                    else
                    {
                        mipmapCount = computeMaxMipmapCount(type, width, height, slices);
                    }
                    break;
            }

            if (nullptr == _importSettings || TextureImporterFormat::Automatic == _importSettings->m_importerFormat)
            {
                // TODO: determine the best format automatically
                format = TextureImporterFormat::RGBA8;
                sRGB = false;
            }

            if (nullptr == _importSettings || MipLevelCount::Automatic == _importSettings->m_mipLevelCount)
            {
                const uint maxMipCount = computeMaxMipmapCount(type, width, height, slices);
                mipmapCount = maxMipCount;                
            }

            // TEMP HACK, TODO: cubemap mipmaps
            //if (type == TextureImporterType::Cubemap)
            //    mipmapCount = 1;
            
            if (_importSettings)
            {
                if (TextureImporterFormat::Automatic != _importSettings->m_importerFormat)
                    format = _importSettings->m_importerFormat;

                sRGB = _importSettings->m_sRGB;

                if (MipLevelCount::Automatic != _importSettings->m_mipLevelCount)
                {
                    const uint maxMipCount = computeMaxMipmapCount(type, width, height, slices);
                    mipmapCount = min((uint)_importSettings->m_mipLevelCount, maxMipCount);
                }
            }

            switch (format)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(format);
                    //break;

                case TextureImporterFormat::RGBA8:
                    if (sRGB)
                        _desc.format = PixelFormat::R8G8B8A8_unorm_sRGB;
                    else
                        _desc.format = PixelFormat::R8G8B8A8_unorm;
                    break;
            }

            _desc.width = width;
            _desc.height = height;
            _desc.slices = slices;
            _desc.mipmaps = mipmapCount;

            generateMipmaps(data, _desc, _buffer);

            VG_SAFE_FREE(data);

            return true;
        }        

        return false;
    }

    //--------------------------------------------------------------------------------------
    void * memset32(void * m, uint32_t val, size_t count)
    {
        u32 * VG_RESTRICT buf = (u32 * VG_RESTRICT)m;

        while (count--) 
            *buf++ = val;
        return m;
    }

    //--------------------------------------------------------------------------------------
    core::u32 TextureImporter::computeMaxMipmapCount(TextureImporterType _type, core::uint _width, core::uint _height, core::uint _depth)
    {
        switch (_type)
        {
            case TextureImporterType::Texture1D:
                return Texture::computeMaxMip(_width);

            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_type);

            case TextureImporterType::Texture2D:
            case TextureImporterType::Cubemap:
                return Texture::computeMaxMip(min(_width, _height));

            case TextureImporterType::Texture3D:
                return Texture::computeMaxMip(min(min(_width, _height), _depth));
        }
    }

    //--------------------------------------------------------------------------------------
    bool TextureImporter::generateMipmaps(const core::u8 * _src, const TextureDesc & _desc, vector<u8> & _buffer)
    {
        uint maxMipCount = Texture::computeMaxMipmapCount(_desc);
        VG_ASSERT(_desc.mipmaps <= maxMipCount);

        const uint width = _desc.width;
        const uint height = _desc.height;

        const auto fmtSize = Texture::getPixelFormatSize(_desc.format);

        size_t totalSize = 0;
        for (uint s = 0; s < _desc.slices; ++s)
        {
            uint mipWidth = width;
            uint mipHeight = height;

            for (uint m = 0; m < _desc.mipmaps; ++m)
            {
                const auto mipSize = mipWidth * mipHeight * fmtSize;
                totalSize += mipSize;
                mipWidth >>= 1;
                mipHeight >>= 1;
            }
        }
        _buffer.resize(totalSize);

        u8 * dst = _buffer.data();

        const u32 mipColor[] =
        {
            0xFF0000FF,
            0xFF00FF00,
            0xFF00FFFF,
            0xFFFF0000,
            0xFFFF00FF,
            0xFFFFFF00
        };

        for (uint s = 0; s < _desc.slices; ++s)
        {
            uint mipWidth = width;
            uint mipHeight = height;

            const u8 * src = _src;
            uint sliceSize = 0;

            for (uint m = 0; m < _desc.mipmaps; ++m)
            {
                const auto mipSize = mipWidth * mipHeight * fmtSize;

                if (m == 0)
                {
                    if (_desc.type == TextureType::TextureCube)
                    {
                        for (uint j = 0; j < mipHeight; ++j)
                        {
                            for (uint i = 0; i < mipWidth; ++i)
                            {
                                const u32 * in = (u32*)_src;
                                u32 * out = (u32*)dst;

                                uint faceOffsetX, faceOffsetY;
                                uint dstX, dstY;
                                switch ((CubemapFace)s)
                                {
                                    // Horizontal cross 
                                    // +-----+-----+-----+-----+  
                                    // |     | Z+  |     |     | 
                                    // |     | 1,0 |     |     |  
                                    // +-----+-----+-----+-----+  
                                    // | X-  | Y-  | X+  | Y+  |  
                                    // | 0,1 | 1,1 | 2,1 | 3,1 |  
                                    // +-----+-----+-----+-----+  
                                    // |     | -Z  |     |     |  
                                    // |     | 1,2 |     |     |  
                                    // +-----+-----+-----+-----+  

                                    case CubemapFace::PositiveX:
                                        faceOffsetX = 2;
                                        faceOffsetY = 1;
                                        dstX = j;
                                        dstY = mipHeight-i-1;
                                        break;

                                    case CubemapFace::NegativeX:
                                        faceOffsetX = 0;
                                        faceOffsetY = 1;
                                        dstX = mipWidth - j - 1;
                                        dstY = i;
                                        break;

                                    case CubemapFace::PositiveY:
                                        faceOffsetX = 3;
                                        faceOffsetY = 1;
                                        dstX = mipWidth - i - 1;
                                        dstY = mipHeight - j - 1;
                                        break;

                                    case CubemapFace::NegativeY:
                                        faceOffsetX = 1;
                                        faceOffsetY = 1;
                                        dstX = i;
                                        dstY = j;
                                        break;

                                    case CubemapFace::PositiveZ:
                                        faceOffsetX = 1;
                                        faceOffsetY = 0;
                                        dstX = i;
                                        dstY = j;
                                        break;

                                    case CubemapFace::NegativeZ:
                                        faceOffsetX = 1;
                                        faceOffsetY = 2;
                                        dstX = mipWidth - i - 1;
                                        dstY = mipHeight - j - 1;
                                        break;
                                }

                                out[dstX + dstY * mipWidth] = in[i + faceOffsetX * mipWidth + (j + faceOffsetY * mipHeight) * mipWidth * 4];
                            }
                        }
                    }
                    else
                    {
                        // TODO: compute src slice index when needed
                        VG_ASSERT(_desc.slices == 1);

                        src = _src;
                        memcpy(dst, src, mipSize);
                    }
                }
                else
                {
                    for (uint j = 0; j < mipHeight; ++j)
                    {
                        for (uint i = 0; i < mipWidth; ++i)
                        {
                            const u32 * in = (u32 *)src;
                            u32 * out = (u32 *)dst;

                            const u32 tex00 = in[((i << 1) + 0) + ((j << 1) + 0) * (mipWidth << 1)];
                            const u32 tex01 = in[((i << 1) + 0) + ((j << 1) + 1) * (mipWidth << 1)];
                            const u32 tex10 = in[((i << 1) + 1) + ((j << 1) + 0) * (mipWidth << 1)];
                            const u32 tex11 = in[((i << 1) + 1) + ((j << 1) + 1) * (mipWidth << 1)];

                            const u16 r = (((tex00 >> 0) & 0xFF) + ((tex01 >> 0) & 0xFF) + ((tex10 >> 0) & 0xFF) + ((tex11 >> 0) & 0xFF)) >> 2;
                            const u16 g = (((tex00 >> 8) & 0xFF) + ((tex01 >> 8) & 0xFF) + ((tex10 >> 8) & 0xFF) + ((tex11 >> 8) & 0xFF)) >> 2;
                            const u16 b = (((tex00 >> 16) & 0xFF) + ((tex01 >> 16) & 0xFF) + ((tex10 >> 16) & 0xFF) + ((tex11 >> 16) & 0xFF)) >> 2;
                            const u16 a = (((tex00 >> 24) & 0xFF) + ((tex01 >> 24) & 0xFF) + ((tex10 >> 24) & 0xFF) + ((tex11 >> 24) & 0xFF)) >> 2;

                            out[i + j * mipWidth] = (a << 24) | (b << 16) | (g << 8) | (r);
                        }
                    }
                }

                src = dst;
                dst += mipSize;
                sliceSize += mipSize;

                mipWidth >>= 1;
                mipHeight >>= 1;
            }
        }

        return true;
    }
}