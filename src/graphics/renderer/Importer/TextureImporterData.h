#pragma once

#include "gfx/Resource/Texture_consts.h"
#include "gfx/Importer/TextureImporter.h"

namespace vg::graphics::renderer
{
    class TextureImporterData
    {
    public:
        bool load(const core::string & _file);
        bool save(const core::string & _file) const;

        core::string                    name;
        gfx::TextureDesc             desc;
        core::vector<core::u8>          texels;
    };
}