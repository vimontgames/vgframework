#pragma once

#include "core/Component/Component.h"
#include "core/Resource/Resource.h"

namespace vg::graphics::renderer
{
    class MeshComponent : public core::Component
    {
    public:
        using super = core::Component;

        const char * getClassName() const final { return "MeshComponent"; }
        static bool registerClass(core::IObjectFactory & _factory);
        static bool registerProperties(core::IObjectDescriptor & _desc);

        MeshComponent();
        ~MeshComponent();

        void update(const core::Entity * _entity, double _dt) override;

    private:
        core::Resource m_model;
    };
}