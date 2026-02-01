#include "physics/Precomp.h"
#include "core/Object/AutoRegisterClass.h"

#include "IPhysicsOptions.h"
#include "IShape.h"
#include "IBody.h"
#include "ICharacter.h"
#include "IVehicleConstraint.h"

//--------------------------------------------------------------------------------------
// Register physics abstract classes
// 
// Each abstract interfaces has to be declared in an object file for RTTI
// Module abstract classes that have no associated .cpp file can be registered here
//--------------------------------------------------------------------------------------
namespace vg::physics
{
    VG_REGISTER_ABSTRACT_CLASS(IPhysicsOptions);
    VG_REGISTER_ABSTRACT_CLASS(IShapeDesc);
    VG_REGISTER_ABSTRACT_CLASS(IShape);
    VG_REGISTER_ABSTRACT_CLASS(ICharacterDesc);
    VG_REGISTER_ABSTRACT_CLASS(ICharacter);
    VG_REGISTER_ABSTRACT_CLASS(IBodyDesc);
    VG_REGISTER_ABSTRACT_CLASS(IBody);
    VG_REGISTER_ABSTRACT_CLASS(IVehicleConstraintDesc);
    VG_REGISTER_ABSTRACT_CLASS(IVehicleConstraint);    
}
