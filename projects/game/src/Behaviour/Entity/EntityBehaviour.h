#pragma once
#include "core/Component/Behaviour/Behaviour.h"

class CharacterBehaviour;
class ItemBehaviour;

class EntityBehaviour : public vg::core::Behaviour
{
public:
    VG_CLASS_DECL(EntityBehaviour, vg::core::Behaviour);

    EntityBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~EntityBehaviour();

    virtual bool TakeHit(CharacterBehaviour * _attacker, ItemBehaviour * _weapon) { return false; }
    virtual void OnGameStateChanged(GameState _newState, GameState _previousState) { }
};