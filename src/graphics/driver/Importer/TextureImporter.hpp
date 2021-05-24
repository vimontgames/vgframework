#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) VG_ASSERT(x)
#include "stb/stb_image.h"
#include "graphics/driver/Resource/Texture.h"

using namespace vg::core;

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    bool TextureImporter::importTextureData(const core::string & _path, TextureDesc & _desc, vector<MipData> & _mips)
    {
        int width, height, channels;
        stbi_uc * data = stbi_load(_path.c_str(), &width, &height, &channels, 0);

        if (nullptr != data)
        {
            _desc.width = width;
            _desc.height = height;
            _desc.format = _desc.format;
            _desc.mipmaps = generateMipmaps(data, _desc, _mips);

            VG_SAFE_FREE(data);

            return true;
        }        

        return false;
    }

    //--------------------------------------------------------------------------------------
    core::uint TextureImporter::generateMipmaps(const core::u8 * _src, const TextureDesc & _desc, vector<MipData> & _mips)
    {
        const uint maxMipCount = 1;// Texture::computeMaxMipmapCount(_desc);

        // Copy mip0 (TODO: compress)
        MipData mip0;
        const size_t bufferSize = _desc.width * _desc.height * Texture::getPixelFormatSize(_desc.format);
        mip0.buffer.resize(bufferSize);
        memcpy(mip0.buffer.data(), _src, bufferSize);

        _mips.push_back(mip0);

        return maxMipCount;
    }
}