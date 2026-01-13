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
    bool IsInstanciateHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const Resource * res = VG_SAFE_STATIC_CAST(const Resource, _object);
        return !res->CanInstanciate();
    }

    //--------------------------------------------------------------------------------------
    bool Resource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEx(Resource, m_resourcePath, "Path", PropertyFlags::Hidden);
        setPropertyDescription(Resource, m_resourcePath, "Resource file relative path");

        // The "Instanciate" flag is not saved by default, but it's up to derived resource 
        // that support it ('CanInstanciate' returns 'true' ) to make it serializable using 
        // e.g. "setPropertyFlag(MaterialResource, m_instanciate, PropertyFlags::Transient, false);"
        registerPropertyEx(Resource, m_instanciate, "Instance", PropertyFlags::Transient);
        setPropertyDescription(Resource, m_instanciate, "A new instance of the resource will be created so as not to modify the original.")
        setPropertyHiddenCallback(Resource, m_instanciate, IsInstanciateHidden);

        // This is used for the inspector to display resource preview, but not serialized
        registerPropertyObjectPtrEx(Resource, m_shared, "Object", PropertyFlags::Transient);

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
        m_shared = _other.m_shared;
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
    void Resource::Swap(IObject * _other)
    {
        super::Swap(_other);

        Resource * other = VG_SAFE_STATIC_CAST(Resource, _other);

        // Notify resource managers that we swapped client pointers
        Kernel::getResourceManager()->SwapResources(this, (IResource*)other);

        swap(m_resourcePath, other->m_resourcePath);
        swap(m_shared, other->m_shared);
        swap(m_userData, other->m_userData);
    }

    //--------------------------------------------------------------------------------------
    Resource::~Resource()
    {
        Kernel::getResourceManager()->UnloadResource(this, GetResourcePath());
		VG_SAFE_RELEASE(m_shared);
        VG_SAFE_RELEASE(m_instance);
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
        if (m_shared != _object)
        {
            VG_SAFE_RELEASE(m_shared);
            VG_SAFE_INCREASE_REFCOUNT(_object);
            m_shared = _object;

            if (m_instanciate)
                Instanciate();

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void Resource::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp("m_instanciate", _prop.GetName()))
        {
            if (m_instanciate)
            {
                if (m_instance == nullptr)
                    Instanciate();
            }
            else
            {
                VG_SAFE_RELEASE(m_instance);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool Resource::Instanciate()
    {
        if (m_shared)
        {
            VG_SAFE_RELEASE(m_instance);

            IFactory * factory = Kernel::getFactory();

            m_instance = factory->CreateObject(m_shared->GetClassDesc()->GetClassName(), m_shared->GetName() + " (Instance)", m_shared);
            VG_ASSERT(nullptr != m_instance);
            if (nullptr == m_instance)
                return false;
           
            factory->CopyProperties(m_shared, m_instance);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    IObject * Resource::GetObject() const
    {
        return m_instance ? m_instance : m_shared;
    }

    //--------------------------------------------------------------------------------------
    IObject * Resource::GetSharedObject() const
    {
        return m_shared;
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
    LoadStatus Resource::Load(const core::string & _file, core::IObject *& _object)
    {
        return LoadStatus::NotImplemented;
    };

    //--------------------------------------------------------------------------------------
    void Resource::Unload(const core::string & _file)
    {
        //VG_INFO("[Resource] Unload %s \"%s\"", GetClassName(), _file.c_str());
        if (auto * object = GetObject())
        {
            object->SetObjectRuntimeFlags(ObjectRuntimeFlags::Unloaded, true);
            object->SetName(object->GetName() + " (Unloaded)");
        }
    }

    //--------------------------------------------------------------------------------------
    void Resource::LoadSubResources()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void Resource::UnloadSubResources()
    {
        
    }


    //--------------------------------------------------------------------------------------
    void Resource::OnResourceLoaded(core::IResource * _resource)
    {

    }

    //--------------------------------------------------------------------------------------
    void Resource::OnResourceUnloaded(core::IResource * _resource)
    {

    }

    //--------------------------------------------------------------------------------------
    IResourceMeta * Resource::CreateResourceMeta(const core::string & _path) const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool Resource::CanOverrideProperties() const
    {
        return false;
    }
}