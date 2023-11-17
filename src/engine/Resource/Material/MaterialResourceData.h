#pragma once

#include "core/Object/Object.h"

namespace vg::engine
{
    class MaterialModelType : public core::Object
    {
    public:
        VG_CLASS_DECL(MaterialModelType, core::Object);
        MaterialModelType(const core::string & _name = "", core::IObject * _parent = nullptr) : core::Object(_name, _parent) {}
        core::string m_materialModelName = "Default";
    };

    class MaterialResourceData : public core::Object
    {
    public:
        VG_CLASS_DECL(MaterialResourceData, core::Object);

        MaterialResourceData(const core::string & _name = "", IObject * _parent = nullptr);
        ~MaterialResourceData();

        MaterialModelType                   m_shader;
        renderer::MaterialFlags             m_flags = (renderer::MaterialFlags)0;
        core::float4                        m_colors[core::enumCount<renderer::MaterialColorType>()];
        TextureResource                     m_textures[core::enumCount<renderer::MaterialTextureType>()];
    };
}