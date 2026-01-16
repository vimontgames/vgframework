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

    public:
        float           m_mass                  = 75.0f;
        float           m_maxSlopeAngle         = 45.0f;
        float           m_maxSeparationDistance = 0.05f;
        float           m_friction              = 0.5f;
    };

    class RigidCharacterDesc final : public CharacterDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(RigidCharacterDesc, CharacterDesc);
        CharacterType GetCharacterType() const final override { return CharacterType::Rigid; }
    };
}