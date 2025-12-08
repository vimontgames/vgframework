#pragma once

#include "core/IResource.h"
#include "gfx/Resource/Texture_consts.h"
#include "gfx/Importer/TextureImporter.h"

namespace vg::renderer
{
    class TextureImporterData
    {
    public:
        core::LoadStatus    load(const core::string & _file);
        bool                save(const core::string & _file) const;

        core::string            name;
        gfx::TextureDesc        desc;
        core::vector<core::u8>  texels;
    };
}