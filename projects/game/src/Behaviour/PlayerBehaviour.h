#pragma once
#include "core/Component/Behaviour/Behaviour.h"

class PlayerBehaviour : public vg::core::Component
{
public:
    VG_CLASS_DECL(PlayerBehaviour, vg::core::Behaviour);

                    PlayerBehaviour     (const vg::core::string & _name, vg::core::IObject * _parent);
                    ~PlayerBehaviour    ();

    void            Update              (double _dt) override;

private:
    vg::core::u8    m_joypadID = 0;
};
