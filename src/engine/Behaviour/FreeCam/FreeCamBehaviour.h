#pragma once

#include "core/Component/Behaviour/Behaviour.h"

namespace vg::renderer
{
    class IView;
}

namespace vg::engine
{
    class CameraComponent;

    class FreeCamBehaviour : public core::Behaviour
    {
    public:
        VG_CLASS_DECL(FreeCamBehaviour, core::Behaviour);

        FreeCamBehaviour        (const core::string & _name, core::IObject * _parent);
        ~FreeCamBehaviour       ();

        void                    Update(float _dt) override;

    private:
        static core::float4x4   getRotX(float _alpha);
        static core::float4x4   getRotY(float _alpha);
        static core::float4x4   getRotZ(float _alpha);

        CameraComponent *       m_camera = nullptr;

        float                   m_pitch;
        float                   m_yaw;
        float                   m_roll;

        float                   m_moveSpeed;
        float                   m_rotSpeed;
    };
}