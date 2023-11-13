#pragma once

#include "core/Resource/Resource.h"
#include "engine/Resource/Texture/TextureResource.h"
#include "renderer/Model/Material/Material_Consts.h"

namespace vg::renderer
{
    class IMaterialModel;
}

namespace vg::engine
{
    class MaterialResource : public core::Resource
    {
    public:
        using super = core::Resource;

        const char *                        getClassName            () const override { return "MaterialResource"; }
        static bool                         registerClass           (core::IFactory & _factory);
        static bool                         registerProperties      (core::IClassDesc & _desc);

        MaterialResource(const core::string & _name = "", IObject * _parent = nullptr);
        ~MaterialResource();

        const core::vector<core::string>    getExtensions           () const final;
        void                                onResourcePathChanged   (const core::string & _oldPath, const core::string & _newPath) final;

        bool                                cook                    (const core::string & _file) const final override;
        core::IObject *                     load                    (const core::string & _path) final override;

        void                                onResourceLoaded        (core::IResource * _resource) final override;
        renderer::IMaterialModel *          getMaterialModel        () const { return (renderer::IMaterialModel*)m_object; }

    //private:
        renderer::MaterialFlags             m_flags = (renderer::MaterialFlags)0;
        core::float4                        m_colors[core::enumCount<renderer::MaterialColorType>()];
        TextureResource                     m_textures[core::enumCount<renderer::MaterialTextureType>()];
    };
}