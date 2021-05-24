#pragma once

#include "graphics/driver/Resource/Texture_consts.h"
#include "graphics/driver/Importer/TextureImporter.h"

namespace vg::graphics::renderer
{
    class TextureImporterData
    {
    public:
        bool load(const core::string & _file);
        bool save(const core::string & _file) const;

        core::string                    name;
        driver::TextureDesc             desc;
        core::vector<driver::MipData>   mips;
    };
}