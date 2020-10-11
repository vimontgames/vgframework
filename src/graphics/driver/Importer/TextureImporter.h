#pragma once

#include "Importer.h"
#include "core/Singleton/Singleton.h"

namespace vg::graphics::driver
{
    class TextureDesc;

    class TextureImporter : public Importer, public core::Singleton<TextureImporter>
    {
    public:
        bool importTextureData(const core::string & _path, TextureDesc & _desc, core::u8 *& _data);
    };
}