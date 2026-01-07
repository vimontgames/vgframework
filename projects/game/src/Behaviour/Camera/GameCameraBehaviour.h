#pragma once
#include "core/Component/Behaviour/Behaviour.h"
#include "core/Object/ObjectHandle.h"
#include "core/Curve/Float1Curve.h"

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
    float                   m_delay                 = 0.1f;
    vg::core::Float1Curve   m_speedToZoomCurve;
    float                   m_zoomDelay             = 1.0f;

    vg::core::float3        m_offset                = vg::core::float3(0.0f, -9.0f, 14.0f);
    vg::core::float3        m_targetPosition        = (vg::core::float3)0.0f;
    vg::core::float3        m_targetVelocity        = (vg::core::float3)0.0f;
    float                   m_currentSpeed          = 0.0f;
    float                   m_currentZoom           = 0.0f;
    float                   m_targetZoomVelocity    = 0.0f;
};
