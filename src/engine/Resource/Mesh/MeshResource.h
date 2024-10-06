#pragma once

#include "core/Resource/Resource.h"
#include "engine/Resource/Material/MaterialResource.h"

namespace vg::renderer
{
    class IMeshModel;
}

namespace vg::engine
{
    class MeshResource : public core::Resource
    {
    public:
        VG_CLASS_DECL(MeshResource, core::Resource);

                                            MeshResource            (const core::string & _name, IObject * _parent);
                                            ~MeshResource           ();

        const core::vector<core::string>    GetExtensions           () const final;
        
        bool                                Cook                    (const core::string & _file) const final override;

        core::IObject *                     Load                    (const core::string & _path) override;
        void                                Unload                  (const core::string & _path) override;

        void                                LoadSubResources        () override;
        void                                UnloadSubResources      () override;

        void                                OnResourceLoaded        (core::IResource * _resource) final override;
        void                                OnResourceUnloaded      (core::IResource * _resource) final override;

        renderer::IMeshModel *              getMeshModel            () const { return (renderer::IMeshModel*)m_object; } 
    };
}