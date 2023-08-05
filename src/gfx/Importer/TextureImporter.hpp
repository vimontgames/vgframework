#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) VG_ASSERT(x)
#include "stb/stb_image.h"
#include "gfx/Resource/Texture.h"

using namespace vg::core;

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    bool TextureImporter::importTextureData(const core::string & _path, TextureDesc & _desc, vector<u8> & _buffer)
    {
        int width, height, channels;
        stbi_uc * data = stbi_load(_path.c_str(), &width, &height, &channels, 0);

        if (nullptr != data)
        {
            _desc.format = PixelFormat::R8G8B8A8_unorm;

            if (string::npos != _path.find("_albedo"))
                _desc.format = PixelFormat::R8G8B8A8_unorm_sRGB;

            _desc.width = width;
            _desc.height = height;
            _desc.mipmaps = generateMipmaps(data, _desc, _buffer);

            VG_SAFE_FREE(data);

            return true;
        }        

        return false;
    }

    void *memset32(void *m, uint32_t val, size_t count)
    {
        u32 * __restrict buf = (u32 *__restrict)m;

        while (count--) 
            *buf++ = val;
        return m;
    }

    //--------------------------------------------------------------------------------------
    core::uint TextureImporter::generateMipmaps(const core::u8 * _src, const TextureDesc & _desc, vector<u8> & _buffer)
    {
        const uint maxMipCount = Texture::computeMaxMipmapCount(_desc);

        uint w = _desc.width;
        uint h = _desc.height;

        size_t totalSize = 0;
        for (uint m = 0; m < maxMipCount; ++m)
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

        for (uint m = 0; m < maxMipCount; ++m)
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