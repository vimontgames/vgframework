#pragma once

#include "core/Entity/Entity.h"

namespace vg::core
{
    class IFactory;
    class IClassDesc;
}

namespace vg::engine
{
    class FreeCamEntity : public core::Entity
    {
        using super = core::Entity;

    public:
        static bool     registerClass       (core::IFactory & _factory);
        static bool     registerProperties  (core::IClassDesc & _desc);

        const char *    getClassName        () const final { return "FreeCamEntity"; }

                        FreeCamEntity       (const core::string & _name, core::IObject * _parent);
                        ~FreeCamEntity      ();

        void            update              (double _dt) override;

    private:
        float m_pitch;
        float m_yaw;
        float m_roll;

        float m_moveSpeed;
        float m_rotSpeed;
    };
}