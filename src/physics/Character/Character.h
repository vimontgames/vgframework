#pragma once

#include "physics/ICharacter.h"

namespace JPH
{
    class CharacterBase;
    class Character;
}

namespace vg::physics
{
    class CharacterDesc;
    class RigidCharacterDesc;
    class Shape;

    class Character : public ICharacter
    {
    public:
        VG_CLASS_DECL(Character, ICharacter);
        VG_CLASS_CTOR_HEADER_IMPL(Character, ICharacter);

        Character(const CharacterDesc * _characterDesc, Shape * _shape, const core::float4x4 & _world);
        ~Character();

        void Activate(const core::float4x4 & _world) = 0;
        void Deactivate(const core::float4x4 & _world) = 0;

        core::float4x4 GetMatrix() const = 0;

    protected:
        JPH::BodyInterface & getBodyInterface() const;

    private:
        const Shape *           m_shape = nullptr;
        const CharacterDesc *   m_characterDesc = nullptr;
    };

    class RigidCharacter : public Character
    {
    public:
        VG_CLASS_DECL(RigidCharacter, Character);
        VG_CLASS_CTOR_HEADER_IMPL(RigidCharacter, Character);

        RigidCharacter(const RigidCharacterDesc * _rigidCharacterDesc, Shape * _shape, const core::float4x4 & _world);
        ~RigidCharacter();

        void            Activate        (const core::float4x4 & _world) final override;
        void            Deactivate      (const core::float4x4 & _world) final override;

        core::float3    GetVelocity     () const final override;
        void            SetVelocity     (const core::float3 & _velocity) final override;

        core::float4x4  GetMatrix       () const final override;

        void            resetCharacter  (const core::float4x4 & _world);

    private:
        JPH::Character * m_character = nullptr;
    };
}