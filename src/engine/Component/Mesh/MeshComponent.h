#pragma once

#include "core/Component/Component.h"
#include "engine/Resource/Mesh/MeshResource.h"
#include "MaterialResourceList.h"

namespace vg::renderer
{
    class IMeshInstance;
}

namespace vg::engine
{
    class MeshComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(MeshComponent, core::Component);

                        MeshComponent       (const core::string & _name, IObject * _parent);
                        ~MeshComponent      ();

        void            Update              (double _dt) override;

        void            onPropertyChanged   (IObject * _object, const core::IProperty & _prop) override;

        void            onResourceLoaded    (core::IResource * _resource) override;
        void            onResourceUnloaded  (core::IResource * _resource) override;

        MeshResource &  getMeshResource     () { return m_meshResource; }

    private:
        bool                                m_displayBones = false;
        renderer::IMeshInstance *           m_meshInstance = nullptr;
        MeshResource                        m_meshResource;
        MaterialResourceList                m_meshMaterials;
        bool                                m_registered = false;
    };
}