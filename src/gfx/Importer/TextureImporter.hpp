#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) VG_ASSERT(x)
#include "stb/stb_image.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Importer/TextureImporterSettings.h"

using namespace vg::core;

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    bool TextureImporter::importTextureData(const core::string & _path, TextureDesc & _desc, vector<u8> & _finalBuffer, const TextureImporterSettings * _importSettings)
    {
        // TODO: support Texture3D/Array loading?
        int srcWidth, srcHeight, srcChannels;

        const bool isHDR = stbi_is_hdr(_path.c_str());
        const bool is16bits = stbi_is_16_bit(_path.c_str());

        void * data = nullptr;
        TextureImporterFormat srcFormat, dstFormat;
        if (isHDR)
        {
            data = (float *)stbi_loadf(_path.c_str(), &srcWidth, &srcHeight, &srcChannels, 4);
            srcFormat = TextureImporterFormat::RGBA32f;
        }
        else if (is16bits)
        {
            data = (u16 *)stbi_load_16(_path.c_str(), &srcWidth, &srcHeight, &srcChannels, 4);
            srcFormat = TextureImporterFormat::RGBA16;
        }
        else
        {
            data = (u8 *)stbi_load(_path.c_str(), &srcWidth, &srcHeight, &srcChannels, 4);
            srcFormat = TextureImporterFormat::RGBA8;
        }

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
                    if (_importSettings && TextureImporterMipLevelCount::Automatic != _importSettings->m_mipLevelCount)
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
                if (isHDR)
                    format = TextureImporterFormat::RGBA32f;
                else if (is16bits)
                    format = TextureImporterFormat::RGBA16;
                else
                    format = TextureImporterFormat::RGBA8;
                sRGB = false;
            }

            if (nullptr == _importSettings || TextureImporterMipLevelCount::Automatic == _importSettings->m_mipLevelCount)
            {
                const uint maxMipCount = computeMaxMipmapCount(type, width, height, slices);
                mipmapCount = maxMipCount;                
            }
            
            if (_importSettings)
            {
                if (TextureImporterFormat::Automatic != _importSettings->m_importerFormat)
                    format = _importSettings->m_importerFormat;

                sRGB = _importSettings->m_sRGB;

                if (TextureImporterMipLevelCount::Automatic != _importSettings->m_mipLevelCount)
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
                    dstFormat = TextureImporterFormat::RGBA8;
                    break;

                case TextureImporterFormat::RGBA16:
                    _desc.format = PixelFormat::R16G16B16A16_unorm;
                    dstFormat = TextureImporterFormat::RGBA16;
                    break;

                case TextureImporterFormat::RGBA32f:
                    _desc.format = PixelFormat::R32G32B32A32_float;
                    dstFormat = TextureImporterFormat::RGBA32f;
                    break;
            }

            _desc.width = width;
            _desc.height = height;
            _desc.slices = slices;
            _desc.mipmaps = mipmapCount;

            bool success = true;

            switch (srcFormat)
            {
                case TextureImporterFormat::RGBA8:
                {
                    vector<UByte4> tempBuffer;
                    switch (dstFormat)
                    {
                        case TextureImporterFormat::RGBA8:
                        {
                            success |= generateMipmaps<UByte4>((UByte4 *)data, _desc, tempBuffer);
                            success |= convert<UByte4, UByte4>(_desc, tempBuffer, _finalBuffer);
                        }
                        break;

                        case TextureImporterFormat::RGBA32f:
                        {
                            success |= generateMipmaps<UByte4>((UByte4 *)data, _desc, tempBuffer);
                            success |= convert<UByte4, float4>(_desc, tempBuffer, _finalBuffer);
                        }
                        break;

                        default:
                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(dstFormat);
                            success = false;
                            break;
                    }
                }
                break;

                case TextureImporterFormat::RGBA16:
                {
                    vector<UShort4> tempBuffer;
                    switch (dstFormat)
                    {
                        case TextureImporterFormat::RGBA32f:
                        {
                            success |= generateMipmaps<UShort4>((UShort4 *)data, _desc, tempBuffer);
                            success |= convert<UShort4, float4>(_desc, tempBuffer, _finalBuffer);
                        }
                        break;

                        case TextureImporterFormat::RGBA16:
                        {
                            success |= generateMipmaps<UShort4>((UShort4 *)data, _desc, tempBuffer);
                            success |= convert<UShort4, UShort4>(_desc, tempBuffer, _finalBuffer);
                        }
                        break;

                        case TextureImporterFormat::RGBA8:
                        {
                            success |= generateMipmaps<UShort4>((UShort4 *)data, _desc, tempBuffer);
                            success |= convert<UShort4, UByte4>(_desc, tempBuffer, _finalBuffer);
                        }
                        break;

                        default:
                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(dstFormat);
                            success = false;
                            break;
                    }
                }
                break;

                case TextureImporterFormat::RGBA32f:
                {
                    vector<float4> tempBuffer;
                    switch (dstFormat)
                    {
                        case TextureImporterFormat::RGBA8:
                        {
                            success |= generateMipmaps<float4>((float4 *)data, _desc, tempBuffer);
                            success |= convert<float4, UByte4>(_desc, tempBuffer, _finalBuffer);
                        }
                        break;

                        case TextureImporterFormat::RGBA16:
                        {
                            success |= generateMipmaps<float4>((float4 *)data, _desc, tempBuffer);
                            success |= convert<float4, UShort4>(_desc, tempBuffer, _finalBuffer);
                        }
                        break;

                        case TextureImporterFormat::RGBA32f:
                        {
                            success |= generateMipmaps<float4>((float4 *)data, _desc, tempBuffer);
                            success |= convert<float4, float4>(_desc, tempBuffer, _finalBuffer);
                        }
                        break;

                        default:
                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(dstFormat);
                            success = false;
                            break;
                    }
                }
                break;

                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(srcFormat);
                    success = false;
                    break;
            }

            VG_SAFE_FREE(data);

            return success;
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
    size_t computeTotalSize(uint _width, uint _height, uint _slices, uint _mipmaps, uint _fmtSize)
    {
        size_t totalSize = 0;
        for (uint s = 0; s < _slices; ++s)
        {
            uint mipWidth = _width;
            uint mipHeight = _height;

            for (uint m = 0; m < _mipmaps; ++m)
            {
                const auto mipSize = mipWidth * mipHeight * _fmtSize;
                totalSize += mipSize;
                mipWidth >>= 1;
                mipHeight >>= 1;
            }
        }
        return totalSize;
    }

    //--------------------------------------------------------------------------------------
    template <typename SourceFormat> bool isHDR(const SourceFormat & _value);

    template <> bool isHDR(const UByte4 & _value) { return false; }
    template <> bool isHDR(const UShort4 & _value) { return false; }
    template <> bool isHDR(const float4 & _value) { return any(_value > 1.0f); }

    //--------------------------------------------------------------------------------------
    template <typename SourceFormat> bool TextureImporter::generateMipmaps(const SourceFormat * _src, const TextureDesc & _desc, core::vector<SourceFormat> & _buffer)
    {
        uint maxMipCount = Texture::computeMaxMipmapCount(_desc);
        VG_ASSERT(_desc.mipmaps <= maxMipCount);

        const uint width = _desc.width;
        const uint height = _desc.height;

        size_t totalSize = computeTotalSize(_desc.width, _desc.height, _desc.slices, _desc.mipmaps, sizeof(SourceFormat));
        _buffer.resize(totalSize);

        SourceFormat * dst = _buffer.data();

        bool hdrValuesFound = false;

        for (uint s = 0; s < _desc.slices; ++s)
        {
            uint mipWidth = width;
            uint mipHeight = height;

            const SourceFormat * src = _src;
            uint sliceSize = 0;

            for (uint m = 0; m < _desc.mipmaps; ++m)
            {
                const uint mipSize = mipWidth * mipHeight;

                if (m == 0)
                {
                    if (_desc.type == TextureType::TextureCube)
                    {
                        for (uint j = 0; j < mipHeight; ++j)
                        {
                            for (uint i = 0; i < mipWidth; ++i)
                            {
                                const SourceFormat * in = _src;
                                SourceFormat * out = dst;

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

                                const auto & srcTexel = in[i + faceOffsetX * mipWidth + (j + faceOffsetY * mipHeight) * mipWidth * 4]; // * 4 because we're using a 4x3 cross pattern
                                auto & dstTexel = out[dstX + dstY * mipWidth];

                                if (!hdrValuesFound)
                                {
                                    if (isHDR(srcTexel))
                                    {
                                        VG_WARNING("[Import] At least one HDR value has been found importing cubemap file");
                                        hdrValuesFound = true;
                                    }
                                }

                                dstTexel.r = srcTexel.r; 
                                dstTexel.g = srcTexel.g; 
                                dstTexel.b = srcTexel.b; 
                                dstTexel.a = srcTexel.a; 
                            }
                        }
                    }
                    else
                    {
                        // TODO: compute src slice index when needed
                        VG_ASSERT(_desc.slices == 1);

                        src = _src;
                        memcpy(dst, src, mipSize * sizeof(SourceFormat));
                    }
                }
                else
                {
                    for (uint j = 0; j < mipHeight; ++j)
                    {
                        for (uint i = 0; i < mipWidth; ++i)
                        {
                            const SourceFormat * in = (SourceFormat *)src;
                            SourceFormat * out = dst;

                            const SourceFormat & tex00 = in[((i << 1) + 0) + ((j << 1) + 0) * (mipWidth << 1)];
                            const SourceFormat & tex01 = in[((i << 1) + 0) + ((j << 1) + 1) * (mipWidth << 1)];
                            const SourceFormat & tex10 = in[((i << 1) + 1) + ((j << 1) + 0) * (mipWidth << 1)];
                            const SourceFormat & tex11 = in[((i << 1) + 1) + ((j << 1) + 1) * (mipWidth << 1)];

                            using scalarType = typename vectorTraits<SourceFormat>::type;

                            out[i + j * mipWidth].r = (tex00.r + tex01.r + tex10.r + tex11.r) / (scalarType)4;
                            out[i + j * mipWidth].g = (tex00.g + tex01.g + tex10.g + tex11.g) / (scalarType)4;
                            out[i + j * mipWidth].b = (tex00.b + tex01.b + tex10.b + tex11.b) / (scalarType)4;
                            out[i + j * mipWidth].a = (tex00.a + tex01.a + tex10.a + tex11.a) / (scalarType)4;
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

    template <typename SourceFormat, typename DestFormat> typename DestFormat store(const typename SourceFormat & _src);

    // Copy without conversion
    template <> typename UByte4 store<UByte4, UByte4>(const UByte4 & _src)      { return _src; }
    template <> typename UShort4 store<UShort4, UShort4>(const UShort4 & _src)  { return _src; }
    template <> typename float4 store<float4, float4>(const float4 & _src)      { return _src; }

    // float4 to UByte4
    template <> typename UByte4 store<float4, UByte4>(const float4 & _src)
    {
        UByte4 value;
        value.r = clamp((u8)(_src.r * 255.0f), (u8)0, (u8)255); 
        value.g = clamp((u8)(_src.g * 255.0f), (u8)0, (u8)255);
        value.b = clamp((u8)(_src.b * 255.0f), (u8)0, (u8)255);
        value.a = clamp((u8)(_src.a * 255.0f), (u8)0, (u8)255);
        return value;
    }

    // float4 to UShort4
    template <> typename UShort4 store<float4, UShort4>(const float4 & _src)
    {
        UShort4 value;
        value.r = clamp((u16)(_src.r * 65535.0f), (u16)0, (u16)65535);
        value.g = clamp((u16)(_src.g * 65535.0f), (u16)0, (u16)65535);
        value.b = clamp((u16)(_src.b * 65535.0f), (u16)0, (u16)65535);
        value.a = clamp((u16)(_src.a * 65535.0f), (u16)0, (u16)65535);
        return value;
    }

    // UShort4 to UByte4
    template <> typename UByte4 store<UShort4, UByte4>(const UShort4 & _src)
    {
        UByte4 value;
        value.r = _src.r >> 8;
        value.g = _src.g >> 8;
        value.b = _src.b >> 8;
        value.a = _src.a >> 8;
        return value;
    }

    // UShort4 to float4
    template <> typename float4 store<UShort4, float4>(const UShort4 & _src)
    {
        float4 value;
        value.r = (float)_src.r / 65535.0f;
        value.g = (float)_src.g / 65535.0f;
        value.b = (float)_src.b / 65535.0f;
        value.a = (float)_src.a / 65535.0f;
        return value;
    }

    // ubyte4 to float4
    template <> typename float4 store<UByte4, float4>(const UByte4 & _src)
    {
        float4 value;
        value.r = (float)_src.r / 255.0f;
        value.g = (float)_src.g / 255.0f;
        value.b = (float)_src.b / 255.0f;
        value.a = (float)_src.a / 255.0f;
        return value;
    }

    //--------------------------------------------------------------------------------------
    template <typename SourceFormat, typename DestFormat> bool TextureImporter::convert(const TextureDesc & _desc, const core::vector<SourceFormat> & _in, core::vector<u8> & _out)
    {
        const uint width = _desc.width;
        const uint height = _desc.height;
        const uint slices = _desc.slices;
        const uint mipmaps = _desc.mipmaps;

        const SourceFormat * pSrc = _in.data();

        const auto totalSize = computeTotalSize(width, height, slices, mipmaps, sizeof(DestFormat));
        _out.resize(totalSize);
        DestFormat * pDst = (DestFormat *)_out.data();

        for (uint s = 0; s < slices; ++s)
        {
            uint mipWidth = width;
            uint mipHeight = height;

            for (uint m = 0; m < mipmaps; ++m)
            {
                for (uint y = 0; y < mipHeight; ++y)
                {
                    for (uint x = 0; x < mipWidth; ++x)
                    {
                        const SourceFormat & src = pSrc[(x + y * mipWidth)];
                        DestFormat & dst = pDst[(x + y * mipWidth)];
                        dst = store<SourceFormat, DestFormat>(src);
                    }
                }

                pSrc += mipWidth * mipHeight;
                pDst += mipWidth * mipHeight;

                mipWidth >>= 1;
                mipHeight >>= 1;                
            }
        }

        return true;
    }
}