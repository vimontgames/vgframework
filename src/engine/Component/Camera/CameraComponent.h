#pragma once

#include "core/Component/Component.h"

namespace vg::core
{
    class IGameObject;
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

        const char * getClassName() const override { return "CameraComponent"; }
        static bool registerClass(core::IFactory & _factory);
        static bool registerProperties(core::IClassDesc & _desc);

        CameraComponent(const core::string & _name, core::IObject * _parent);
        ~CameraComponent();

        void Update(double _dt) override;

        VG_INLINE float getFovY() const { return m_fovY; }
        VG_INLINE float getNear() const { return m_near; }
        VG_INLINE float getFar() const { return m_far; }

    private:

        float m_fovY;
        float m_near;
        float m_far;
    };
}