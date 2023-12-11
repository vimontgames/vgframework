#pragma once

#include "physics/ICharacter.h"

namespace vg::physics
{
    class CharacterDesc : public ICharacterDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(CharacterDesc, ICharacterDesc);

        virtual CharacterType GetCharacterType() const = 0;

        void SetMass(float _mass) final override                    { m_mass = _mass; }
        void SetMaxSlopeAngle(float _maxSlopeAngle) final override  { m_maxSlopeAngle = _maxSlopeAngle;}
        //void SetFriction(float _friction) final override            { m_friction = _friction;}

    public:
        float           m_mass          = 87.0f;
        float           m_maxSlopeAngle = 45.0f;
        float           m_friction      = 0.5f;
        //float           m_restitution   = 0.0f;
    };

    class RigidCharacterDesc final : public CharacterDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(RigidCharacterDesc, CharacterDesc);
        CharacterType GetCharacterType() const final override { return CharacterType::Rigid; }
    };
}