//--------------------------------------------------------------------------------------
VG_INLINE CharacterType CharacterBehaviour::getCharacterType() const
{
    return m_characterType;
}

//--------------------------------------------------------------------------------------
VG_INLINE bool CharacterBehaviour::isActive() const
{
    return m_isActive; 
}

//--------------------------------------------------------------------------------------
VG_INLINE MoveState CharacterBehaviour::getMoveState() const
{
    return m_moveState;
}

//--------------------------------------------------------------------------------------
VG_INLINE FightState CharacterBehaviour::getFightState() const
{
    return m_fightState;
}

//--------------------------------------------------------------------------------------
VG_INLINE float CharacterBehaviour::getHealth() const
{
    return m_hp;
}