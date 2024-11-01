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
        int width, height, slices, channels;
        stbi_uc * data = stbi_load(_path.c_str(), &width, &height, &channels, 4);

        // TODO: support Texture3D/Array loading
        slices = 1;

        if (nullptr != data)
        {
            TextureImporterType type;
            TextureImporterFormat format;
            bool sRGB;
            core::uint mipmapCount;

            if (nullptr == _importSettings || TextureImporterFormat::Automatic == _importSettings->m_importerFormat)
                type = TextureImporterType::Texture2D;

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
            
            if (_importSettings)
            {
                if (TextureImporterType::Automatic != _importSettings->m_importerType)
                    type = _importSettings->m_importerType;

                if (TextureImporterFormat::Automatic != _importSettings->m_importerFormat)
                    format = _importSettings->m_importerFormat;


                sRGB = _importSettings->m_sRGB;

                if (MipLevelCount::Automatic != _importSettings->m_mipLevelCount)
                {
                    const uint maxMipCount = computeMaxMipmapCount(type, width, height, slices);
                    mipmapCount = min((uint)_importSettings->m_mipLevelCount, maxMipCount);
                }
            }

            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    //break;

                case TextureImporterType::Texture2D:
                    _desc.type = TextureType::Texture2D;
                    break;
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

            _desc.mipmaps = generateMipmaps(data, _desc, _buffer, type, format, mipmapCount);

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
    core::uint TextureImporter::generateMipmaps(const core::u8 * _src, const TextureDesc & _desc, vector<u8> & _buffer, TextureImporterType _type, TextureImporterFormat _format, core::uint _mipmapCount)
    {
        uint maxMipCount = Texture::computeMaxMipmapCount(_desc);
        VG_ASSERT(_mipmapCount <= maxMipCount);
        _mipmapCount = min((uint)_mipmapCount, maxMipCount);

        uint w = _desc.width;
        uint h = _desc.height;

        size_t totalSize = 0;
        for (uint m = 0; m < _mipmapCount; ++m)
        {
            const auto mipSize = w * h * Texture::getPixelFormatSize(_desc.format);
            totalSize += mipSize;
            w >>= 1;
            h >>= 1;
        }
        _buffer.resize(totalSize);

        w = _desc.width;
        h = _desc.height;

        const u8 * src = _src;
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

        for (uint m = 0; m < _mipmapCount; ++m)
        {
            const auto mipSize = w * h * Texture::getPixelFormatSize(_desc.format);

            if (m == 0)
            {
                memcpy(dst, src, mipSize);
            }
            else
            {
                for (uint j = 0; j < h; ++j)
                {
                    for (uint i = 0; i < w; ++i)
                    {
                        const u32 * in = (u32*)src;
                        u32 * out = (u32*)dst;

                        const u32 tex00 = in[((i << 1) + 0) + ((j << 1) + 0)*(w << 1)];
                        const u32 tex01 = in[((i << 1) + 0) + ((j << 1) + 1)*(w << 1)];
                        const u32 tex10 = in[((i << 1) + 1) + ((j << 1) + 0)*(w << 1)];
                        const u32 tex11 = in[((i << 1) + 1) + ((j << 1) + 1)*(w << 1)];

                        const u16 r = (((tex00 >>  0) & 0xFF) + ((tex01 >>  0) & 0xFF) + ((tex10 >>  0) & 0xFF) + ((tex11 >>  0) & 0xFF)) >> 2;
                        const u16 g = (((tex00 >>  8) & 0xFF) + ((tex01 >>  8) & 0xFF) + ((tex10 >>  8) & 0xFF) + ((tex11 >>  8) & 0xFF)) >> 2;
                        const u16 b = (((tex00 >> 16) & 0xFF) + ((tex01 >> 16) & 0xFF) + ((tex10 >> 16) & 0xFF) + ((tex11 >> 16) & 0xFF)) >> 2;
                        const u16 a = (((tex00 >> 24) & 0xFF) + ((tex01 >> 24) & 0xFF) + ((tex10 >> 24) & 0xFF) + ((tex11 >> 24) & 0xFF)) >> 2;

                        out[i + j * w] = (a<<24) | (b<<16) | (g<<8) | (r);
                    }
                }
            }

            src = dst;
            dst += mipSize;

            w >>= 1;
            h >>= 1;
        }

        return maxMipCount;
    }
}