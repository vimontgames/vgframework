#pragma once
#include "Behaviour/Character/CharacterBehaviour.h"
#include "core/Object/ObjectHandle.h"

class PlayerBehaviour final : public CharacterBehaviour
{
public:
    VG_CLASS_DECL(PlayerBehaviour, CharacterBehaviour);

    PlayerBehaviour             (const vg::core::string & _name, vg::core::IObject * _parent);
    ~PlayerBehaviour            ();

    void                        OnPlay              () final override;
    void                        OnStop              () final override;

    void                        FixedUpdate         (const Context & _context) override;
    void                        Update              (const Context & _context) override;

    const vg::core::float4 &    getCustomColor      () const { return m_customColor; }

private:
    vg::core::InputType         m_controllerType    = vg::core::InputType::Joypad;
    vg::core::u8                m_controllerIndex   = 0;
    vg::core::float4            m_customColor       = vg::core::float4(1, 1, 1, 1);
    vg::core::ObjectHandle      m_UI;
    ItemBehaviour *             m_rightHandItem = nullptr;
};
