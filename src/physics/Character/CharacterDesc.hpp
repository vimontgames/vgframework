#include "CharacterDesc.h"

using namespace vg::core;

namespace vg::physics
{
    VG_REGISTER_ABSTRACT_CLASS(CharacterDesc);

    //--------------------------------------------------------------------------------------
    bool CharacterDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(CharacterDesc, m_mass, "Mass");
        setPropertyRange(CharacterDesc, m_mass, float2(0, 100000));

        registerProperty(CharacterDesc, m_friction, "Friction");
        setPropertyRange(CharacterDesc, m_friction, float2(0, 1));

        registerProperty(CharacterDesc, m_maxSlopeAngle, "MaxSlope");
        setPropertyRange(CharacterDesc, m_maxSlopeAngle, float2(0, 90));
        setPropertyDescription(CharacterDesc, m_maxSlopeAngle, "Maximum angle of slope that character can still walk on.");

        registerProperty(CharacterDesc, m_maxSeparationDistance, "MaxSeparationDist");
        setPropertyRange(CharacterDesc, m_maxSeparationDistance, float2(0, 1));
        setPropertyDescription(CharacterDesc, m_maxSeparationDistance, "Max distance between the floor and the character to still consider the character standing on the floor.")

        return true;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_CLASS(RigidCharacterDesc, "RigidCharacter Settings");

    //--------------------------------------------------------------------------------------
    bool RigidCharacterDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);        

        return true;
    }
}