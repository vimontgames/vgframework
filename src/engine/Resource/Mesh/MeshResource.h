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
        using super = core::Resource;

        const char *                        getClassName            () const override { return "MeshResource"; }
        static bool                         registerClass           (core::IFactory & _factory);
        static bool                         registerProperties      (core::IClassDesc & _desc);

                                            MeshResource            (const core::string & _name, IObject * _parent);
                                            ~MeshResource           ();

        const core::vector<core::string>    getExtensions           () const final;
        void                                onResourcePathChanged   (const core::string & _oldPath, const core::string & _newPath) final;

        bool                                cook                    (const core::string & _file) const final override;

        core::IObject *                     load                    (const core::string & _path) override;
        void                                unload                  (const core::string & _path) override;

        void                                loadSubResources        () override;
        void                                unloadSubResources      () override;

        void                                onResourceLoaded        (core::IResource * _resource) final override;
        void                                onResourceUnloaded      (core::IResource * _resource) final override;

        renderer::IMeshModel *              getMeshModel            () const { return (renderer::IMeshModel*)m_object; } 

    private:
        //core::vector<MaterialResource>      m_materialResources;
    };
}