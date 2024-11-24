#include "Batch.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/File/Buffer.h"

using namespace vg::core;

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS(Batch, "Batch");

    //--------------------------------------------------------------------------------------
    bool Batch::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(Batch, m_name, PropertyFlags::Hidden, false);
        setPropertyFlag(Batch, m_name, PropertyFlags::Debug, true);

        registerPropertyEx(Batch, count, "Count", PropertyFlags::ReadOnly);
        setPropertyDescription(Batch, count, "Index or vertex count");

        registerPropertyEx(Batch, offset, "Offset", PropertyFlags::ReadOnly);
        setPropertyDescription(Batch, offset, "Start index or start vertex");

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
        SetName(_other.GetName());
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
        string name;
        _buffer.read(&name);
        SetName(name);
        _buffer.read(&offset);
        _buffer.read(&count);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Batch::write(io::Buffer & _buffer) const
    {
        _buffer.write(GetName());
        _buffer.write(offset);
        _buffer.write(count);

        return true;
    }
}