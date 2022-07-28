#include "core/Precomp.h"
#include "Resource.h"
#include "core/File/File.h"
#include "core/Object/AutoRegisterClass.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool Resource::registerClass(IFactory & _factory)
    {
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Resource::registerProperties(IClassDesc & _desc)
    {
        _desc.registerProperty("m_object", (IObject**)(&((Resource*)(nullptr))->m_object), "Object", IProperty::Flags::None);
        return true;
    }

    //--------------------------------------------------------------------------------------
    Resource::Resource(const core::string & _name, IObject * _parent) :
        m_owner(_parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Resource::Resource(const Resource & _other)
    {
        setName(_other.getName());

        m_path = _other.m_path;
        m_owner = _other.m_owner;
        m_object = _other.m_object;
        m_userData = _other.m_userData;
    }

    //--------------------------------------------------------------------------------------
    Resource::~Resource()
    {
		VG_SAFE_RELEASE(m_object);
    }

    //--------------------------------------------------------------------------------------
    void Resource::setup(IObject * _owner, const string & _path, UserData _userData)
    {
        setOwner(_owner);
        setUserData(_userData);
        setPath(_path);
    }

    //--------------------------------------------------------------------------------------
    bool Resource::setObject(core::IObject * _object)
    {
        if (m_object != _object)
        {
            VG_SAFE_RELEASE(m_object);
            VG_SAFE_INCREASE_REFCOUNT(_object);
            m_object = _object;

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    IObject * Resource::getObject() const
    {
        return m_object;
    }

    //--------------------------------------------------------------------------------------
    void Resource::setUserData(UserData _userData)
    {
        m_userData = _userData;
    }

    //--------------------------------------------------------------------------------------
    IResource::UserData Resource::getUserData() const
    {
        return m_userData;
    }    

    //--------------------------------------------------------------------------------------
    bool Resource::setPath(const string & _path)
    {
        VG_ASSERT(nullptr != getOwner());

        if (m_path == _path)
            return false;
     
        string oldPath = m_path;
        m_path = io::getRelativePath(_path);
        onPathChanged(m_owner, oldPath, m_path);

        return true;
    }

    //--------------------------------------------------------------------------------------
    const string & Resource::getPath() const
    {
        return m_path;
    }

    //--------------------------------------------------------------------------------------
    void Resource::setOwner(core::IObject * _object)
    {
        m_owner = _object;
    }

    //--------------------------------------------------------------------------------------
    core::IObject * Resource::getOwner() const
    {
        return m_owner;
    }
}