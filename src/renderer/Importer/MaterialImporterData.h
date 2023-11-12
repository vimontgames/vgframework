#pragma once

#include "renderer/Model/Material/Material_Consts.h"

namespace vg::core::io
{
    class Buffer;
}

namespace vg::renderer
{
    class MaterialImporterData
    {
    public:

        bool read(core::io::Buffer & _buffer);
        bool write(core::io::Buffer & _buffer) const;

        core::string name;
        core::string texturePath[core::enumCount<MaterialTextureType>()];
    };
}