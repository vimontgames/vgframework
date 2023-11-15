#pragma once

#include "core/Object/Object.h"

namespace vg::engine
{
    class MaterialResourceData : public core::Object
    {
    public:
        using super = core::Object;

        const char *                        getClassName        () const override { return "MaterialResourceData"; }
        static bool                         registerClass       (core::IFactory & _factory);
        static bool                         registerProperties  (core::IClassDesc & _desc);

        MaterialResourceData(const core::string & _name = "", IObject * _parent = nullptr);
        ~MaterialResourceData();

        void                                onResourceLoaded    (core::IResource * _resource) final override;
        void                                onResourceUnloaded  (IResource * _resource) final override;

        renderer::MaterialFlags             m_flags = (renderer::MaterialFlags)0;
        core::float4                        m_colors[core::enumCount<renderer::MaterialColorType>()];
        TextureResource                     m_textures[core::enumCount<renderer::MaterialTextureType>()];
    };
}