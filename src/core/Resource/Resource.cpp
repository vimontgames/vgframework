#include "core/Precomp.h"
#include "Resource.h"
#include "core/File/File.h"
#include "core/Object/AutoRegisterClass.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool Resource::registerClass(IObjectFactory & _factory)
    {
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Resource::registerProperties(IObjectDescriptor & _desc)
    {
        _desc.registerProperty("m_object", (IObject**)(&((Resource*)(nullptr))->m_object), "Object", IPropertyDescriptor::Flags::None);
        return true;
    }

    //--------------------------------------------------------------------------------------
    Resource::Resource(IObject * _owner) :
        m_owner(_owner)
    {

    }

    //--------------------------------------------------------------------------------------
    Resource::~Resource()
    {

    }

    //--------------------------------------------------------------------------------------
    IObject * Resource::get() const 
    { 
        return m_object; 
    }

    //--------------------------------------------------------------------------------------
    const string & Resource::getPath() const
    {
        return m_path;
    }

    //--------------------------------------------------------------------------------------
    bool Resource::setPath(const string & _path)
    {
        if (m_path == _path)
            return false;
     
        string oldPath = m_path;
        m_path = getRelativePath(_path);
        onPathChanged(m_owner, oldPath, m_path);

        return true;
    }


}