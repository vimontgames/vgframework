#pragma once

#include "core/GameObject/GameObject.h"

namespace vg::core
{
    class IFactory;
    class IClassDesc;
}

namespace vg::engine
{
    class FreeCam : public core::GameObject
    {
        using super = core::GameObject;

    public:
        static bool     registerClass       (core::IFactory & _factory);
        static bool     registerProperties  (core::IClassDesc & _desc);

        const char *    getClassName        () const final { return "FreeCam"; }

                        FreeCam             (const core::string & _name, core::IObject * _parent);
                        ~FreeCam            ();

        void            Update              (double _dt) override;

    private:
        float           m_pitch;
        float           m_yaw;
        float           m_roll;

        float           m_moveSpeed;
        float           m_rotSpeed;
    };
}