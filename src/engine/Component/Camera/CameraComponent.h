#pragma once

#include "core/Component/Component.h"

namespace vg::graphics::renderer
{
    class IView;
}

namespace vg::engine
{
    class CameraComponent : public core::Component
    {
    public:
        using super = core::Component;

        const char * getClassName() const final { return "CameraComponent"; }
        static bool registerClass(core::IObjectFactory & _factory);
        static bool registerProperties(core::IObjectDescriptor & _desc);

        CameraComponent();
        ~CameraComponent();

        void update(const core::Entity * _entity, double _dt) override;

    public:
        void setView(graphics::renderer::IView * _view);

    private:
        graphics::renderer::IView * m_view = nullptr;
    };
}