#pragma once

#include "graphics/driver/Resource/Texture_consts.h"

namespace vg::graphics::renderer
{
    class TextureImporterData
    {
    public:
        bool load(const core::string & _file);
        bool save(const core::string & _file) const;

        core::string            name;
        driver::TextureDesc     desc;
        core::u32               size = 0;
        core::u8 *              data = nullptr;
    };
}