#pragma once

#include "core/Component/Component.h"

namespace vg::core
{
    class ISector;
}

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

        CameraComponent(const core::string & _name, core::IObject * _parent);
        ~CameraComponent();

        void update(double _dt) override;

        float getFovY() const { return m_fovY; }
        float getNear() const { return m_near; }
        float getFar() const { return m_far; }

    public:
        void setView(graphics::renderer::IView * _view, core::ISector * _sector);

    private:
        graphics::renderer::IView * m_view = nullptr;

        float m_fovY;
        float m_near;
        float m_far;
    };
}