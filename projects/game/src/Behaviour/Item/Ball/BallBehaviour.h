#pragma once
#include "Behaviour/Item/ItemBehaviour.h"

class CharacterBehaviour;

class BallBehaviour : public ItemBehaviour
{
public:
    VG_CLASS_DECL(BallBehaviour, ItemBehaviour);

    BallBehaviour(const vg::core::string & _name, vg::core::IObject * _parent);
    ~BallBehaviour();

    void                    OnPlay              () final override;
    void                    OnCollisionEnter    (vg::core::IGameObject * _other) override;
    void                    Update              (const Context & _context) override;

    void                    onGoalScored        (CharacterType _team);

private:
    CharacterBehaviour *    m_lastOwnerCharacter = nullptr;
    vg::core::float3        m_startPos;
    bool                    m_scored = false;
    float                   m_scoredTime = -1.0f;
};