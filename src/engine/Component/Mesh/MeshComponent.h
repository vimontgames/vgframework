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
        static bool     registerClass       (core::IObjectFactory & _factory);
        static bool     registerProperties  (core::IObjectDescriptor & _desc);

                        MeshComponent       (const core::string & _name, IObject * _parent);
                        ~MeshComponent      ();

        void            update              (double _dt) override;
        void            onResourceLoaded    (core::IResource * _resource) override;

    private:
        graphics::renderer::IMeshInstance * m_meshInstance = nullptr;
        MeshResource                        m_meshResource;
    };
}