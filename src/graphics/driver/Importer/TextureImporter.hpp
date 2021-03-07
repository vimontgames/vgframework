#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) VG_ASSERT(x)
#include "stb/stb_image.h"
#include "graphics/driver/Resource/Texture_consts.h"

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    bool TextureImporter::importTextureData(const core::string & _path, TextureDesc & _desc, core::u8 *& _data)
    {
        int width, height, channels;
        stbi_uc * data = stbi_load(_path.c_str(), &width, &height, &channels, 0);

        if (data)
        {
            _data = data;
            _desc.width = width;
            _desc.height = height;
            _desc.format = _desc.format;

            return true;
        }

        return false;
    }
}