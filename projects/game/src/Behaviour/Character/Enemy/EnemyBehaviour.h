#pragma once
#include "Behaviour/Character/CharacterBehaviour.h"

class EnemyBehaviour final : public CharacterBehaviour
{
public:
    VG_CLASS_DECL(EnemyBehaviour, CharacterBehaviour);

    EnemyBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~EnemyBehaviour();

    void                OnPlay() final override;
    void                OnStop() final override;

    void                FixedUpdate(float _dt) override;
    void                Update(float _dt) override;

private:

};
