#pragma once
#include "core/Component/Component.h"

class PlayerComponent : vg::core::Component
{
    using super = vg::core::Component;

public:
    const char *    getClassName        () const final { return "PlayerComponent"; }
    static bool     registerClass       (vg::core::IFactory & _factory);
    static bool     registerProperties  (vg::core::IClassDesc & _desc);

                    PlayerComponent     (const vg::core::string & _name, IObject * _parent);
                    ~PlayerComponent    ();

    void            Update              (double _dt) override;

private:
    float           m_MyFloat = 42.0f;
};
