#pragma once

#include "core/Entity/Entity.h"

namespace vg::engine
{
    class FreeCamEntity : public core::Entity
    {
        using super = core::Entity;

    public:
        const char * getClassName() const final { return "FreeCamEntity"; }

        FreeCamEntity(const core::string & _name);
        ~FreeCamEntity();

        void update(double _dt) override;

    private:
        float m_pitch;
        float m_yaw;
        float m_roll;
    };
}