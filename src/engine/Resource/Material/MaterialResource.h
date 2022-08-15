#pragma once

#include "core/Resource/Resource.h"
#include "engine/Resource/Texture/TextureResource.h"

namespace vg::graphics::renderer
{
    class IMaterialModel;
}

namespace vg::engine
{
    class MaterialResource : public core::Resource
    {
    public:
        using super = core::Resource;

        const char *                        getClassName        () const override { return "MaterialResource"; }
        static bool                         registerClass       (core::IFactory & _factory);
        static bool                         registerProperties  (core::IClassDesc & _desc);

        MaterialResource(const core::string & _name = "", IObject * _parent = nullptr);
        ~MaterialResource();

        const core::vector<core::string>    getExtensions       () const final;
        void                                onPathChanged       (IObject * _owner, const core::string & _oldPath, const core::string & _newPath) final;

        bool                                cook                (const core::string & _file) final override;
        bool                                load                (const core::string & _path, core::IObject * _owner) final override;

        void                                onResourceLoaded    (core::IResource * _resource) final override;
        graphics::renderer::IMaterialModel *getMaterialModel    () const { return (graphics::renderer::IMaterialModel*)m_object; }

        core::uint                          getSubResourceCount () const final;
        IResource *                         getSubResource      (core::uint _index) final;

    //private:
        core::vector<TextureResource>       m_textureResources;
    };
}