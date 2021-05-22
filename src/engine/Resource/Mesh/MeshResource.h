#pragma once

#include "core/Resource/Resource.h"
#include "engine/Resource/Texture/TextureResource.h"

namespace vg::graphics::renderer
{
    class IMeshModel;
}

namespace vg::engine
{
    class MeshResource : public core::Resource
    {
    public:
        MeshResource(IObject * _owner);
        ~MeshResource();

        const core::vector<core::string>    getExtensions       () const final;
        void                                onPathChanged       (IObject * _owner, const core::string & _oldPath, const core::string & _newPath) final;

        bool                                cook                (const core::string & _file) final override;
        bool                                load                (const core::string & _path, core::IObject * _owner) final override;

        void                                onResourceLoaded    (core::IResource * _resource) final override;
        graphics::renderer::IMeshModel *    getMeshModel        () const { return (graphics::renderer::IMeshModel*)m_object; }        

    private:
        struct MaterialResource
        {
            core::vector<TextureResource>   m_textureResources;
        };
        core::vector<MaterialResource>      m_materialResources;
    };
}