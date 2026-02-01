#include "audio/Precomp.h"
#include "core/Object/AutoRegisterClass.h"

#include "IAudioOptions.h"
#include "ISound.h"

//--------------------------------------------------------------------------------------
// Register audio abstract classes
// 
// Each abstract interfaces has to be declared in an object file for RTTI
// Module abstract classes that have no associated .cpp file can be registered here
//--------------------------------------------------------------------------------------
namespace vg::audio
{
    VG_REGISTER_ABSTRACT_CLASS(IAudioOptions);
    VG_REGISTER_ABSTRACT_CLASS(ISound);
}
