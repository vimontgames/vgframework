#pragma once
#include "core/Component/Behaviour/Behaviour.h"

class HealthBarBehaviour final : public vg::core::Behaviour
{
public:
    VG_CLASS_DECL(HealthBarBehaviour, vg::core::Behaviour);

    HealthBarBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~HealthBarBehaviour();

    void                OnPlay() final override;
    void                OnStop() final override;

    void                Update(const Context & _context) final override;

    void                init(float _maxHP);
    void                setHP(float _currentHP);

private:
    float               m_width = 0;
    float               m_maxHP = 0;
    float               m_currentHP = 0;
};
