#pragma once
#include "Behaviour/Entity/Character/CharacterBehaviour.h"
#include "core/Object/ObjectHandle.h"

class PlayerBehaviour final : public CharacterBehaviour
{
public:
    VG_CLASS_DECL(PlayerBehaviour, CharacterBehaviour);

    PlayerBehaviour                     (const vg::core::string & _name, vg::core::IObject * _parent);
    ~PlayerBehaviour                    ();

    void                                OnPlay              () final override;
    void                                OnStop              () final override;

    void                                FixedUpdate         (const Context & _context) override;
    void                                Update              (const Context & _context) override;

    bool                                enterVehicle        (vg::core::IGameObject * _vehicleGameobject);
    bool                                exitVehicle         ();

    VG_INLINE const vg::core::float4 &  getCustomColor      () const;
    VG_INLINE const vg::core::u8        getViewportIndex    () const;

private:
    vg::core::InputType                 m_controllerType    = vg::core::InputType::Joypad;
    vg::core::u8                        m_controllerIndex   = 0;
    vg::core::float4                    m_customColor       = vg::core::float4(1, 1, 1, 1);
    vg::core::ObjectHandle              m_UI;
    ItemBehaviour *                     m_rightHandItem     = nullptr;
    vg::core::u8                        m_viewIndex         = -1;
    float                               m_nextShootTime     = 0.0f;
    float                               m_respawnTime       = 0.0f;
};

#if VG_ENABLE_INLINE
#include "PlayerBehaviour.inl"
#endif