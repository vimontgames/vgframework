#pragma once

#include "Importer.h"
#include "core/Singleton/Singleton.h"

namespace vg::graphics::driver
{
    class TextureDesc;

    class TextureImporter : public Importer, public core::Singleton<TextureImporter>
    {
    public:
        bool importTextureData(const core::string & _path, TextureDesc & _desc, core::vector<core::u8> & _buffer);
        core::uint generateMipmaps(const core::u8 * _src, const TextureDesc & _desc, core::vector<core::u8> & _buffer);
    };
}