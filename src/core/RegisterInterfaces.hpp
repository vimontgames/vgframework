#include "core/Precomp.h"
#include "core/Object/AutoRegisterClass.h"

#include "IComponent.h"
#include "IBehaviour.h"

//--------------------------------------------------------------------------------------
// Register core abstract classes
// 
// Each abstract interfaces has to be declared in an object file for RTTI
// Module abstract classes that have no associated .cpp file can be registered here
//--------------------------------------------------------------------------------------
namespace vg::core
{
    VG_REGISTER_ABSTRACT_CLASS(IComponent);
    VG_REGISTER_ABSTRACT_CLASS(IBehaviour);
}