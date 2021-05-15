#include "Batch.h"
#include "core/Object/AutoRegisterClass.h"

using namespace vg::core;

namespace vg::graphics::renderer
{
    VG_AUTO_REGISTER_CLASS(Batch);

    //--------------------------------------------------------------------------------------
    bool Batch::registerClass(core::IObjectFactory & _factory)
    {
        if (core::IObjectDescriptor * desc = _factory.registerClassHelper(Batch, "Batch", IObjectDescriptor::Flags::None))
            registerProperties(*desc);

        return true;
    }


    //--------------------------------------------------------------------------------------
    bool Batch::registerProperties(core::IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyHelper(Batch, count, "Count", IPropertyDescriptor::Flags::ReadOnly);
        _desc.registerPropertyHelper(Batch, offset, "Offset", IPropertyDescriptor::Flags::ReadOnly);

        return true;
    }

    //--------------------------------------------------------------------------------------
    Batch::Batch(const core::string & _name, core::IObject * _parent) :
        core::Object(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Batch::Batch(const Batch & _other)
    {
        setName(_other.getName());
        offset = _other.offset;
        count = _other.count;
    }

    //--------------------------------------------------------------------------------------
    Batch::~Batch()
    {
       
    }
}