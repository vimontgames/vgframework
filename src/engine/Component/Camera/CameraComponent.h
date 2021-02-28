#pragma once

#include "core/Component/Component.h"

namespace vg::graphics::renderer
{
    class IView;
}

namespace vg::engine
{
    class CameraComponent : public vg::core::Component
    {
    public:
        CameraComponent();
        ~CameraComponent();

        void update(const core::Entity * _entity, double _dt) override;

    public:
        void setView(graphics::renderer::IView * _view);

    private:
        graphics::renderer::IView * m_view = nullptr;
    };
}