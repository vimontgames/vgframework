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

        MaterialImporterData();

        bool read(core::io::Buffer & _buffer);
        bool write(core::io::Buffer & _buffer) const;

        core::string m_name;
        core::string m_texturePath[core::enumCount<MaterialTextureType>()];
        core::float4 m_colors[core::enumCount<MaterialColorType>()];
    };
}