#pragma once

#include "core/Entity/Entity.h"

namespace vg::core
{
    class IObjectFactory;
    class IObjectDescriptor;
}

namespace vg::engine
{
    class FreeCamEntity : public core::Entity
    {
        using super = core::Entity;

    public:
        static bool     registerClass       (core::IObjectFactory & _factory);
        static bool     registerProperties  (core::IObjectDescriptor & _desc);

        const char *    getClassName        () const final { return "FreeCamEntity"; }

                        FreeCamEntity       (const core::string & _name = "");
                        ~FreeCamEntity      ();

        void            update              (double _dt) override;

    private:
        float m_pitch;
        float m_yaw;
        float m_roll;
    };
}