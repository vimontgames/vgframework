#pragma once

#include "physics/ICharacter.h"

namespace vg::core
{
    class IWorld;
}

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
    class PhysicsWorld;

    class Character : public ICharacter
    {
    public:
        VG_CLASS_DECL(Character, ICharacter);
        VG_CLASS_CTOR_HEADER_IMPL(Character, ICharacter);

        Character(PhysicsWorld * _physicsWorld, const CharacterDesc * _characterDesc, Shape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent);
        ~Character();

        void                    Activate        (const core::float4x4 & _world) override = 0;
        void                    Deactivate      (const core::float4x4 & _world) override = 0;

        core::float4x4          GetMatrix       () const override = 0;

        GroundState             GetGroundState  () const override = 0;

    protected:
        const PhysicsWorld *    getPhysicsWorld () const { return m_physicsWorld;}

    private:
        const PhysicsWorld *    m_physicsWorld = nullptr;
        const Shape *           m_shape = nullptr;
        const CharacterDesc *   m_characterDesc = nullptr;
    };

    class RigidCharacter final : public Character
    {
    public:
        VG_CLASS_DECL(RigidCharacter, Character);
        VG_CLASS_CTOR_HEADER_IMPL(RigidCharacter, Character);

        RigidCharacter(PhysicsWorld * _physicsWorld, const RigidCharacterDesc * _rigidCharacterDesc, Shape * _shape, const core::float4x4 & _matrix, const core::string & _name, core::IObject * _parent);
        ~RigidCharacter();

        void                    Activate        (const core::float4x4 & _world) final override;
        void                    Deactivate      (const core::float4x4 & _world) final override;

        core::float3            GetVelocity     () const final override;
        void                    SetVelocity     (const core::float3 & _velocity) final override;
        void                    SetPosition     (const core::float3 & _position) final override;
        void                    SetRotation     (const core::quaternion & _rotation) final override;

        void                    FixedUpdate     () final override;
        void                    Update          () final override;

        core::float4x4          GetMatrix       () const final override;
        GroundState             GetGroundState  () const final override;
        bool                    CanMoveTo       (const core::float3 & _position) final override;

    private:
        void                    resetCharacter  (const core::float4x4 & _world);

    private:
        JPH::Character *        m_character = nullptr;
    };
}