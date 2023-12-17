#pragma once

#include "engine/ICharacterControllerComponent.h"
#include "physics/Physics_Consts.h"

namespace vg::engine
{
    class CharacterControllerComponent final : public ICharacterControllerComponent
    {
    public:
        VG_CLASS_DECL(CharacterControllerComponent, ICharacterControllerComponent);
        CharacterControllerComponent(const core::string & _name, IObject * _parent);
        ~CharacterControllerComponent();

        physics::GroundState        GetGroundState      () const final override;

        core::float3                GetVelocity         () const final override;
        void                        SetVelocity         (const core::float3 & _velocity) final override;
        void                        SetRotation         (const core::quaternion & _rotation) final override;

        void                        FixedUpdate         (float _dt) final override;
        void                        Update              (float _dt) final override;

        void                        OnLoad              () final override;

        void                        OnPlay              () final override;
        void                        OnStop              () final override;

        void                        OnPropertyChanged   (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

    private:
        static physics::IPhysics *  getPhysics          ();

        bool                        createShapeDesc     ();
        bool                        createShape         ();
        bool                        createCharacterDesc ();
        bool                        createCharacter     ();

    private:
        // settings
        physics::CharacterType      m_characterType = physics::CharacterType::Rigid;
        physics::IShapeDesc *       m_shapeDesc     = nullptr;
        physics::IShape *           m_shape         = nullptr;
        physics::ICharacterDesc *   m_characterDesc = nullptr;
        physics::ICharacter *       m_character     = nullptr;

        // debug
        physics::GroundState         m_groundState  = (physics::GroundState)-1;
    };
}