#include "CharacterDesc.h"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    bool CharacterDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(CharacterDesc, m_mass, "Mass");

        registerProperty(CharacterDesc, m_friction, "Friction");
        setPropertyRange(CharacterDesc, m_friction, float2(0, 1));

        //registerProperty(CharacterDesc, m_restitution, "Restitution");
        //setPropertyRange(CharacterDesc, m_restitution, float2(0, 1));

        registerProperty(CharacterDesc, m_maxSlopeAngle, "MaxSlope");
        setPropertyRange(CharacterDesc, m_maxSlopeAngle, float2(0, 90));

        return true;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(RigidCharacterDesc, "RigidCharacter Settings");
    //--------------------------------------------------------------------------------------
    bool RigidCharacterDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        

        return true;
    }
}