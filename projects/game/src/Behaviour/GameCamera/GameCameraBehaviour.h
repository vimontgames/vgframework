#pragma once
#include "core/Component/Behaviour/Behaviour.h"

class GameCameraBehaviour : public vg::core::Behaviour
{
public:
    VG_CLASS_DECL(GameCameraBehaviour, vg::core::Behaviour);

    GameCameraBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~GameCameraBehaviour();

    void                OnPlay      () final override;
    void                Update      (float _dt) override;

private:
    float               m_speed             = 1.0f;
    vg::core::float3    m_offset            = (vg::core::float3)0.0f;
    vg::core::float3    m_target            = (vg::core::float3)0.0f;
    vg::core::float3    m_targetVelocity    = (vg::core::float3)0.0f;
};
