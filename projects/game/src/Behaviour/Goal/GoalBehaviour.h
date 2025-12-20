#pragma once
#include "core/Component/Behaviour/Behaviour.h"

class GoalBehaviour : public vg::core::Behaviour
{
public:
    VG_CLASS_DECL(GoalBehaviour, vg::core::Behaviour);

    GoalBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~GoalBehaviour();

    void                OnEnable() final override;
    void                Update(const Context & _context) override;
    void                OnTriggerEnter(vg::core::IGameObject * _other) override;

private:
    CharacterType       m_team = CharacterType::Neutral;
};
