#pragma once
#include "core/Component/Behaviour/Behaviour.h"

class PlayerBehaviour : vg::core::Component
{
public:
    VG_CLASS_DECL(PlayerBehaviour, vg::core::Behaviour);

                    PlayerBehaviour     (const vg::core::string & _name, IObject * _parent);
                    ~PlayerBehaviour    ();

    void            Update              (double _dt) override;

private:
    float           m_MyFloat = 42.0f;
};
