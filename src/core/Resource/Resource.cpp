#include "core/Precomp.h"
#include "Resource.h"
#include "core/File/File.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Kernel.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(Resource, "Resource");

    //--------------------------------------------------------------------------------------
    bool Resource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEx(Resource, m_resourcePath, "Path", IProperty::Flags::NotVisible);
        setPropertyDescription(Resource, m_resourcePath, "Resource file relative path");

        // This is used for the inspector but not serialized
        registerPropertyObjectPtrEx(Resource, m_object, "Object", IProperty::Flags::NotSaved);

        return true;
    }

    //--------------------------------------------------------------------------------------
    Resource::Resource(const core::string & _name, IObject * _parent) : 
        IResource(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Resource::Resource(const Resource & _other) : IResource(_other)
    {
        setName(_other.getName());

        m_resourcePath = _other.m_resourcePath;
        m_object = _other.m_object;
        m_userData = _other.m_userData;
    }

    //--------------------------------------------------------------------------------------
    Resource & Resource::operator=(const vg::core::Resource & _from)
    {
        setName(_from.getName());

        setParent(_from.getParent());
        SetResourcePath(_from.GetResourcePath());
        
        return *this;
    }

    //--------------------------------------------------------------------------------------
    Resource::~Resource()
    {
		VG_SAFE_RELEASE(m_object);
    }

    //--------------------------------------------------------------------------------------
    bool Resource::CreateFile(const string & _path, IObject * _data)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Resource::SaveFile(const string & _path) const
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return false;
    }

    //--------------------------------------------------------------------------------------
    void Resource::setup(IObject * _owner, const string & _path, UserData _userData)
    {
        setObject(nullptr);
        setParent(_owner);
        setUserData(_userData);
        SetResourcePath(_path);
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
    bool Resource::ClearResourcePath()
    {
        return SetResourcePath("");
    }

    //--------------------------------------------------------------------------------------
    bool Resource::SetResourcePath(const string & _path)
    {
        if (m_resourcePath == _path)
            return false;

        IObject * parent = getParent();
        
        if (nullptr != parent)
        {
            string oldPath = m_resourcePath;
            m_resourcePath = io::getRelativePath(_path);
            onResourcePathChanged(oldPath, m_resourcePath);

            return true;
        }

        VG_ASSERT(nullptr != parent, "Resource \"%s\" has no parent", _path.c_str());
        return false;
    }

    //--------------------------------------------------------------------------------------
    const string & Resource::GetResourcePath() const
    {
        return m_resourcePath;
    }

    //--------------------------------------------------------------------------------------
    void Resource::unload(const core::string & _file)
    {
        //VG_INFO("[Resource] Unload %s \"%s\"", GetClassName(), _file.c_str());
    }

    //--------------------------------------------------------------------------------------
    void Resource::loadSubResources()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void Resource::unloadSubResources()
    {
        
    }
}