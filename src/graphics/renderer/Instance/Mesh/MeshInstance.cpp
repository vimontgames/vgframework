#include "graphics/renderer/Precomp.h"
#include "MeshInstance.h"
#include "core/Object/AutoRegisterClass.h"

namespace vg::graphics::renderer
{
    VG_AUTO_REGISTER_CLASS(MeshInstance);

    //--------------------------------------------------------------------------------------
    bool MeshInstance::registerClass(core::IObjectFactory & _factory)
    {
        if (core::IObjectDescriptor * desc = _factory.registerClassHelper(MeshInstance, "Mesh Instance"))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::registerProperties(core::IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MeshInstance::MeshInstance(const core::string & _name, core::IObject * _parent) :
        IMeshInstance(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    MeshInstance::~MeshInstance()
    {

    }
}