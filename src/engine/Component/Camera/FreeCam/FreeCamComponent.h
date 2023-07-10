#pragma once

#include "engine/Component/Camera/CameraComponent.h"

namespace vg::graphics::renderer
{
    class IView;
}

namespace vg::engine
{
    class FreeCamComponent : public CameraComponent
    {
    public:
        using super = CameraComponent;

        const char* getClassName() const final { return "FreeCamComponent"; }
        static bool registerClass(core::IFactory& _factory);
        static bool registerProperties(core::IClassDesc& _desc);

        FreeCamComponent(const core::string & _name, core::IObject * _parent);
        ~FreeCamComponent();

        void Update(double _dt) override;

    protected:
        static core::float4x4 getRotX(float _alpha);
        static core::float4x4 getRotY(float _alpha);
        static core::float4x4 getRotZ(float _alpha);

    private:
        float           m_pitch;
        float           m_yaw;
        float           m_roll;

        float           m_moveSpeed;
        float           m_rotSpeed;
    };
}