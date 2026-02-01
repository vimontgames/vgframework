#include "core/Precomp.h"
#include "core/Object/AutoRegisterClass.h"

#include "IOptions.h"
#include "IComponent.h"
#include "IBehaviour.h"

#include "IBaseScene.h"
#include "IBaseSceneRenderData.h"

#include "IDynamicProperty.h"
#include "IDynamicPropertyList.h"

#include "IGameObject.h"
#include "IInstance.h"
#include "IModel.h"

#include "IObjectList.h"

#include "IResource.h"
#include "IResourceMeta.h"
#include "IResourceList.h"

#include "IWorld.h"


//--------------------------------------------------------------------------------------
// Register core abstract classes
// 
// Each abstract interfaces has to be declared in an object file for RTTI
// Module abstract classes that have no associated .cpp file can be registered here
//--------------------------------------------------------------------------------------
namespace vg::core
{
    VG_REGISTER_ABSTRACT_CLASS(IOptions);

    VG_REGISTER_ABSTRACT_CLASS(IComponent);
    VG_REGISTER_ABSTRACT_CLASS(IBehaviour);

    VG_REGISTER_ABSTRACT_CLASS(IBaseScene);
    VG_REGISTER_ABSTRACT_CLASS(IBaseSceneRenderData);

    VG_REGISTER_ABSTRACT_CLASS(IDynamicProperty);
    VG_REGISTER_ABSTRACT_CLASS(IDynamicPropertyList);

    VG_REGISTER_ABSTRACT_CLASS(IGameObject);
    VG_REGISTER_ABSTRACT_CLASS(IInstance);
    VG_REGISTER_ABSTRACT_CLASS(IModel);

    VG_REGISTER_ABSTRACT_CLASS(IObjectList);

    VG_REGISTER_ABSTRACT_CLASS(IResource);
    VG_REGISTER_ABSTRACT_CLASS(IResourceMeta);
    VG_REGISTER_ABSTRACT_CLASS(IResourceList);

    VG_REGISTER_ABSTRACT_CLASS(IWorld);
}
