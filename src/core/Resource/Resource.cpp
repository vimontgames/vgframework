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
        _desc.registerPropertyHelperEx(Resource, m_resourcePath, "Path", IProperty::Flags::Hidden);

        // This is used for the inspector but does not need to be serialized
        _desc.registerProperty("Resource", "m_object", (IObject**)(&((Resource*)(nullptr))->m_object), "Object", IProperty::Flags::NotSaved);

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

        m_resourcePath = _other.m_resourcePath;
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
        setResourcePath(_path);
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
    bool Resource::setResourcePath(const string & _path)
    {
        VG_ASSERT(nullptr != getOwner());

        if (m_resourcePath == _path)
            return false;
     
        string oldPath = m_resourcePath;
        m_resourcePath = io::getRelativePath(_path);
        onResourcePathChanged(m_owner, oldPath, m_resourcePath);

        return true;
    }

    //--------------------------------------------------------------------------------------
    const string & Resource::getResourcePath() const
    {
        return m_resourcePath;
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