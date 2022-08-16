#pragma once

#include "core/Component/Component.h"
#include "engine/Resource/Mesh/MeshResource.h"

namespace vg::graphics::renderer
{
    class IMeshInstance;
}

namespace vg::engine
{
    class MeshComponent : public core::Component
    {
    public:
        using super = core::Component;

        const char *    getClassName        () const final { return "MeshComponent"; }
        static bool     registerClass       (core::IFactory & _factory);
        static bool     registerProperties  (core::IClassDesc & _desc);

                        MeshComponent       (const core::string & _name, IObject * _parent);
                        ~MeshComponent      ();

        void            Update              (double _dt) override;
        void            onResourceLoaded    (core::IResource * _resource) override;

        MeshResource &  getMeshResource     () { return m_meshResource; }

    private:
        graphics::renderer::IMeshInstance * m_meshInstance = nullptr;
        MeshResource                        m_meshResource;
        bool                                m_registered = false;
    };
}