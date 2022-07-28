#include "graphics/renderer/Precomp.h"
#include "MeshInstance.h"
#include "core/Object/AutoRegisterClass.h"

namespace vg::graphics::renderer
{
    VG_AUTO_REGISTER_CLASS(MeshInstance);

    //--------------------------------------------------------------------------------------
    bool MeshInstance::registerClass(core::IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MeshInstance, "Mesh Instance", core::IClassDesc::Flags::Instance))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshInstance::registerProperties(core::IClassDesc & _desc)
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