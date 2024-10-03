#pragma once
#include "core/Component/Behaviour/Behaviour.h"
#include "core/Object/ObjectHandle.h"

class GameCameraBehaviour : public vg::core::Behaviour
{
public:
    VG_CLASS_DECL(GameCameraBehaviour, vg::core::Behaviour);

    GameCameraBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~GameCameraBehaviour();

    void                    OnEnable    () final override;
    void                    Update      (const Context & _context) override;

    void                    setTarget   (const vg::core::IGameObject * _target);

private:
    vg::core::ObjectHandle  m_target;
    float                   m_delay             = 0.1f;

    vg::core::float3        m_offset            = vg::core::float3(0.0f, -9.0f, 14.0f);
    vg::core::float3        m_targetPosition    = (vg::core::float3)0.0f;
    vg::core::float3        m_targetVelocity    = (vg::core::float3)0.0f;
};
