#include "engine/Precomp.h"
#include "core/Object/AutoRegisterClass.h"

#include "IEngineOptions.h"

#include "IUICanvasComponent.h"
#include "IUICanvasComponent.h"
#include "IUIImageComponent.h"
#include "IUITextComponent.h"

#include "IAnimationComponent.h"
#include "IMeshComponent.h"
#include "IParticleComponent.h"

#include "ISoundComponent.h"

#include "IPhysicsObjectComponent.h"
#include "IPhysicsBodyComponent.h"
#include "IPhysicsShapeComponent.h"
#include "ICharacterControllerComponent.h"
#include "IPhysicsConstraintComponent.h"
#include "IAttachToNodeComponent.h"
#include "IVehicleComponent.h"

#include "IMaterialResource.h"
#include "IAnimationResource.h"
#include "ISoundResource.h"
#include "IWorldResource.h"

#include "ISnapComponent.h"
#include "ICameraComponent.h"

//--------------------------------------------------------------------------------------
// Register engine abstract classes
// 
// Each abstract interfaces has to be declared in an object file for RTTI
// Module abstract classes that have no associated .cpp file can be registered here
//--------------------------------------------------------------------------------------
namespace vg::engine
{
    VG_REGISTER_ABSTRACT_CLASS(IEngineOptions);

    VG_REGISTER_ABSTRACT_CLASS(IUIComponent);
    VG_REGISTER_ABSTRACT_CLASS(IUICanvasComponent);
    VG_REGISTER_ABSTRACT_CLASS(IUIImageComponent);
    VG_REGISTER_ABSTRACT_CLASS(IUITextComponent);

    VG_REGISTER_ABSTRACT_CLASS(IAnimationComponent); 
    VG_REGISTER_ABSTRACT_CLASS(IMeshComponent);   
    VG_REGISTER_ABSTRACT_CLASS(IParticleComponent);

    VG_REGISTER_ABSTRACT_CLASS(ISoundComponent);    
    
    VG_REGISTER_ABSTRACT_CLASS(IPhysicsObjectComponent);
    VG_REGISTER_ABSTRACT_CLASS(IPhysicsBodyComponent);
    VG_REGISTER_ABSTRACT_CLASS(IPhysicsShapeComponent);
    VG_REGISTER_ABSTRACT_CLASS(ICharacterControllerComponent);
    VG_REGISTER_ABSTRACT_CLASS(IPhysicsConstraintComponent);
    VG_REGISTER_ABSTRACT_CLASS(IAttachToNodeComponent);
    VG_REGISTER_ABSTRACT_CLASS(IVehicleComponent);

    VG_REGISTER_ABSTRACT_CLASS(IMaterialResource);
    VG_REGISTER_ABSTRACT_CLASS(IAnimationResource);
    VG_REGISTER_ABSTRACT_CLASS(ISoundResource);
    VG_REGISTER_ABSTRACT_CLASS(IWorldResource);

    VG_REGISTER_ABSTRACT_CLASS(ISnapComponent);
    VG_REGISTER_ABSTRACT_CLASS(ICameraComponent);
}