#include "core/Precomp.h"
#include "Resource.h"
#include "core/File/File.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Kernel.h"
#include "core/IResourceManager.h"

#include "ResourceMeta.hpp"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(Resource, "Resource");

    //--------------------------------------------------------------------------------------
    bool Resource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEx(Resource, m_resourcePath, "Path", PropertyFlags::NotVisible);
        setPropertyDescription(Resource, m_resourcePath, "Resource file relative path");

        // This is used for the inspector but not serialized
        registerPropertyObjectPtrEx(Resource, m_object, "Object", PropertyFlags::NotSaved);
        //registerPropertyObjectPtrEx(Resource, m_meta, "Metadata", PropertyFlags::NotVisible);

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
        SetName(_other.GetName());

        m_resourcePath = _other.m_resourcePath;
        m_object = _other.m_object;
        m_userData = _other.m_userData;
    }

    //--------------------------------------------------------------------------------------
    Resource & Resource::operator=(const vg::core::Resource & _from)
    {
        SetName(_from.GetName());

        SetParent(_from.GetParent());
        SetResourcePath(_from.GetResourcePath());
        
        return *this;
    }

    //--------------------------------------------------------------------------------------
    Resource::~Resource()
    {
        Kernel::getResourceManager()->UnloadResource(this, GetResourcePath());
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
    void Resource::Setup(IObject * _owner, const string & _path, UserData _userData)
    {
        SetObject(nullptr);
        SetParent(_owner);
        SetUserData(_userData);
        SetResourcePath(_path);
    }

    //--------------------------------------------------------------------------------------
    bool Resource::SetObject(core::IObject * _object)
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
    IObject * Resource::GetObject() const
    {
        return m_object;
    }

    //--------------------------------------------------------------------------------------
    void Resource::SetUserData(UserData _userData)
    {
        m_userData = _userData;
    }

    //--------------------------------------------------------------------------------------
    IResource::UserData Resource::GetUserData() const
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

        IObject * parent = GetParent();
        
        if (nullptr != parent)
        {
            string oldPath = m_resourcePath;
            m_resourcePath = io::getRelativePath(_path);
            OnResourcePathChanged(oldPath, m_resourcePath);

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
    const vector<string> Resource::GetExtensions() const 
    { 
        vector<string> ext; return ext; 
    };

    //--------------------------------------------------------------------------------------
    void Resource::OnResourcePathChanged(const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            Kernel::getResourceManager()->LoadResourceAsync(this, _oldPath, _newPath);
    }

    //--------------------------------------------------------------------------------------
    // Default cook is to simply copy the file: Cooked file is same format as source file for now (TODO : serializeFrom/ToBinary)
    //--------------------------------------------------------------------------------------
    bool Resource::Cook(const core::string & _file) const
    { 
        const string cookedPath = io::getCookedPath(_file);

        string data;
        if (io::readFile(_file, data))
        {
            if (io::writeFile(cookedPath, data))
            {
                return true;
            }
        }

        return false;
    };

    //--------------------------------------------------------------------------------------
    core::IObject * Resource::Load(const core::string & _file)
    {
        return nullptr;
    };

    //--------------------------------------------------------------------------------------
    void Resource::Unload(const core::string & _file)
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

    //--------------------------------------------------------------------------------------
    IResourceMeta * Resource::CreateResourceMeta(const core::string & _path) const
    {
        return nullptr;
    }
}