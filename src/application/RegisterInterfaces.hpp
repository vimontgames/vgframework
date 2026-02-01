#include "application/Precomp.h"
#include "core/Object/AutoRegisterClass.h"

#include "IGameOptions.h"

//--------------------------------------------------------------------------------------
// Register application abstract classes
// 
// Each abstract interfaces has to be declared in an object file for RTTI
// Module abstract classes that have no associated .cpp file can be registered here
//--------------------------------------------------------------------------------------
namespace vg
{
    VG_REGISTER_ABSTRACT_CLASS(IGameOptions);
}
