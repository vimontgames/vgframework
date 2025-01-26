#pragma once
#include "Behaviour/Entity/Character/CharacterBehaviour.h"

class EnemyBehaviour final : public CharacterBehaviour
{
public:
    VG_CLASS_DECL(EnemyBehaviour, CharacterBehaviour);

    EnemyBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~EnemyBehaviour();

    void                OnPlay() final override;
    void                OnStop() final override;
    void                OnTriggerEnter(vg::core::IGameObject * _other) override;
    void                FixedUpdate(const Context & _context) override;
    void                Update(const Context & _context) override;

private:
    float               m_attackRadius = 1.0f;
    float               m_detectionRadius = 10.0f;
    vg::core::float3    m_targetPosNew = vg::core::float3(0, 0, 0);
    vg::core::float3    m_targetPosSmooth = vg::core::float3(0, 0, 0);
    bool                m_targetAcquired = false;
    vg::core::float3    m_targetPosSmoothdamp = vg::core::float3(0, 0, 0);
    vg::core::Tag       m_playerTag = (vg::core::Tag)0x0;
};
