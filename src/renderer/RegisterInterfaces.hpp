#include "renderer/Precomp.h"
#include "core/Object/AutoRegisterClass.h"

#include "IRendererOptions.h"
#include "ISkeleton.h"
#include "ISkeletalAnimation.h"
#include "ICameraLens.h"
#include "ICameraSettings.h"
#include "IGraphicInstance.h"
#include "IMeshInstance.h"
#include "IParticleSystemInstance.h"
#include "ILightInstance.h"
#include "ICameraInstance.h"
#include "IMaterialModel.h"

//--------------------------------------------------------------------------------------
// Register renderer abstract classes
// 
// Each abstract interfaces has to be declared in an object file for RTTI
// Module abstract classes that have no associated .cpp file can be registered here
//--------------------------------------------------------------------------------------
namespace vg::renderer
{
    VG_REGISTER_ABSTRACT_CLASS(IRendererOptions);
    VG_REGISTER_ABSTRACT_CLASS(ISkeleton);
    VG_REGISTER_ABSTRACT_CLASS(IAnimation);
    VG_REGISTER_ABSTRACT_CLASS(ISkeletalAnimation);
    VG_REGISTER_ABSTRACT_CLASS(ICameraLens);
    VG_REGISTER_ABSTRACT_CLASS(ICameraSettings);
    VG_REGISTER_ABSTRACT_CLASS(IGraphicInstance);
    VG_REGISTER_ABSTRACT_CLASS(IMeshInstance);
    VG_REGISTER_ABSTRACT_CLASS(IParticleSystemInstance);
    VG_REGISTER_ABSTRACT_CLASS(ILightDesc);
    VG_REGISTER_ABSTRACT_CLASS(ILightInstance);
    VG_REGISTER_ABSTRACT_CLASS(ICameraInstance);
    VG_REGISTER_ABSTRACT_CLASS(IMaterialModel);
}
