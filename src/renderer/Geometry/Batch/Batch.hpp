#include "Batch.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/File/Buffer.h"

using namespace vg::core;

namespace vg::renderer
{
    VG_AUTO_REGISTER_CLASS(Batch);

    //--------------------------------------------------------------------------------------
    bool Batch::registerClass(core::IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(Batch, "Batch", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }


    //--------------------------------------------------------------------------------------
    bool Batch::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyHelperEx(Batch, count, "Count", IProperty::Flags::ReadOnly);
        _desc.registerPropertyHelperEx(Batch, offset, "Offset", IProperty::Flags::ReadOnly);

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

    //--------------------------------------------------------------------------------------
    bool Batch::read(io::Buffer & _buffer)
    {
        _buffer.read(&offset);
        _buffer.read(&count);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Batch::write(io::Buffer & _buffer) const
    {
        _buffer.write(offset);
        _buffer.write(count);

        return true;
    }
}