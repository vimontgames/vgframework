#include "core/Precomp.h"
#include "Object.h"
#include "core/ISelection.h"
#include "core/Kernel.h"
#include "core/File/File.h"
#include "core/Resource/Resource.h"

#if !VG_ENABLE_INLINE
#include "Object.inl"
#endif

#include "Property.hpp"
#include "ClassDesc.hpp"
#include "AutoRegisterClass.hpp"
#include "ObjectHandle.hpp"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool IsOriginalUIDHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const Object * object = VG_SAFE_STATIC_CAST(const Object, _object);
        return 0x0 == object->GetOriginalUID(false);
    }

    //--------------------------------------------------------------------------------------
    bool Object::registerProperties(IClassDesc & _desc)
    {
        registerPropertyEx(Object, m_name, "Name", PropertyFlags::Hidden);
        setPropertyDescription(Object, m_name, "User-friendly name");

        registerPropertyEx(Object, m_uid, "Object UID", PropertyFlags::UID | PropertyFlags::Hexadecimal | PropertyFlags::ReadOnly);
        setPropertyDescription(Object, m_uid, "Object unique ID");

        // TODO: display only if != 0?
        registerPropertyEx(Object, m_originalUID, "Source UID", PropertyFlags::UID | PropertyFlags::Hexadecimal | PropertyFlags::ReadOnly);
        setPropertyDescription(Object, m_originalUID, "Source object's unique ID");
        setPropertyHiddenCallback(Object, m_originalUID, IsOriginalUIDHidden);

        registerPropertyEnumBitfieldEx(Object, ObjectFlags, m_objectFlags, "Object Flags", PropertyFlags::Debug);
        setPropertyDescription(Object, m_objectFlags, "Object flags that are serialized");

        registerPropertyEnumBitfieldEx(Object, ObjectRuntimeFlags, m_objectRuntimeFlags, "Runtime flags", PropertyFlags::Runtime | PropertyFlags::Transient);
        setPropertyDescription(Object, m_objectRuntimeFlags, "Object flags that are only runtime");

        return true;
    }

	//--------------------------------------------------------------------------------------
	Object::Object(const core::string & _name, IObject * _parent) :
		m_refCount(1)
	{
		SetName(_name);
        SetParent(_parent);
	}

    //--------------------------------------------------------------------------------------
    Object & Object::operator=(const Object & _from)
    {
        m_refCount = 1;

        m_uid = _from.m_uid;
        m_originalUID = _from.m_originalUID;
        m_objectFlags = _from.m_objectFlags;
        m_parent = _from.m_parent;

        #ifndef VG_FINAL
        m_parentNameDbg = _from.m_parentNameDbg;
        #endif

        m_name = _from.m_name;
        m_file = _from.m_file;
        m_classDesc = _from.m_classDesc;

        return *this;
    }

    //--------------------------------------------------------------------------------------
    template<typename T>  void atomic_swap(std::atomic<T> & a, std::atomic<T> & b)
    {
        T old_a = a.exchange(b.load(std::memory_order_acquire));
        b.store(old_a, std::memory_order_release);
    }

    //--------------------------------------------------------------------------------------
    void Object::Swap(IObject * _other)
    {
        Object * other = VG_SAFE_STATIC_CAST(Object,_other);
        atomic_swap(m_refCount, other->m_refCount);

        // Swap UIDs
        {
            auto uid = m_uid;
            auto originalUid = m_originalUID;

            auto otherUid = other->m_uid;
            auto otherOriginalUid = other->m_originalUID;

            UnregisterUID();
            _other->UnregisterUID();

            SetUID(otherUid);
            SetOriginalUID(otherOriginalUid);

            _other->SetUID(uid);
            _other->SetOriginalUID(originalUid);
        }

        swap(m_parent, other->m_parent);

        #ifndef VG_FINAL
        swap(m_parentNameDbg, other->m_parentNameDbg);
        #endif

        swap(m_name, other->m_name);
        swap(m_file, other->m_file);
        swap(m_classDesc, other->m_classDesc);
    }

	//--------------------------------------------------------------------------------------
	Object::Object() :
        m_refCount(1)
	{
		
	}

    //--------------------------------------------------------------------------------------
    Object::Object(const Object & _other)
    {
        m_name = _other.m_name;
        m_file = _other.m_file;
        m_refCount.store(_other.m_refCount.load());
        m_parent = _other.m_parent;
    }

	//--------------------------------------------------------------------------------------
	Object::~Object()
	{
        VG_ASSERT(m_refCount <= 1, "An object is being deleted with a RefCount of %u\nUse VG_SAFE_RELEASE for refcounted objects", m_refCount.load());
        UnregisterUID();
	}

    //--------------------------------------------------------------------------------------
    bool Object::RegisterUID()
    {
        IFactory * factory = Kernel::getFactory();

        SetUID(factory->RegisterUID(this));
        return HasValidUID();
    }

    //--------------------------------------------------------------------------------------
    bool Object::UnregisterUID()
    {
        if (m_uid)
        {
            IFactory * factory = Kernel::getFactory();
            factory->ReleaseUID(this, m_uid);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Object::HasValidUID() const
    {
        return (UID)0 != m_uid;
    }

    //--------------------------------------------------------------------------------------
    UID Object::GetUID(bool _mustBeValid) const
    {
        VG_ASSERT(!_mustBeValid || (UID)0 != m_uid, "Object \"%s\" has no UID", GetName().c_str());
        return m_uid;
    }

    //--------------------------------------------------------------------------------------
    void Object::SetUID(UID _uid)
    {
        if (_uid != m_uid)
        {
            if (m_uid)
            {
                IFactory * factory = Kernel::getFactory();
                factory->ReleaseUID(this, m_uid);
            }
            m_uid = _uid;
            RegisterUID();
        }
    }

    //--------------------------------------------------------------------------------------
    UID Object::GetOriginalUID(bool _mustBeValid) const
    {
        VG_ASSERT(!_mustBeValid || (UID)0 != m_originalUID, "Object \"%s\" has no original UID", GetName().c_str());
        return m_originalUID;
    }

    //--------------------------------------------------------------------------------------
    void Object::SetOriginalUID(UID _uid)
    {
        if (_uid != m_originalUID)
            m_originalUID = _uid;
    }

    //--------------------------------------------------------------------------------------
    ObjectFlags Object::GetObjectFlags() const 
    {
        return getObjectFlags();
    }

    //--------------------------------------------------------------------------------------
    void Object::SetObjectFlags(ObjectFlags _flags, bool _enabled)
    {
        setObjectFlags(_flags, _enabled);
    }

    //--------------------------------------------------------------------------------------
    ObjectRuntimeFlags Object::GetObjectRuntimeFlags() const
    {
        return getObjectRuntimeFlags();
    }

    //--------------------------------------------------------------------------------------
    void Object::SetObjectRuntimeFlags(ObjectRuntimeFlags _flags, bool _enabled)
    {
        setObjectRuntimeFlags(_flags, _enabled);
    }

    //--------------------------------------------------------------------------------------
    // If target object has the same original UID than the original UID referenced in the 
    // prefab, then it's our new target.
    //--------------------------------------------------------------------------------------
    const IObject * findByOriginalUID(const Object * _object, UID _originalUID)
    {
        if (_object->GetOriginalUID(false) == _originalUID)
            return _object;

        if (const auto * classDesc = _object->GetClassDesc())
        {
            const auto propCount = classDesc->GetPropertyCount();
            for (uint i = 0; i < propCount; ++i)
            {
                const auto * prop = classDesc->GetPropertyByIndex(i);
                switch (prop->GetType())
                {
                    default:
                        break;

                    case PropertyType::ObjectHandle:
                    {
                        if (const ObjectHandle * objHandle = prop->GetPropertyObjectHandle(_object))
                        {
                            if (const auto * obj = objHandle->getObject())
                            {
                                if (obj != _object)
                                {
                                    if (auto * found = findByOriginalUID((Object *)obj, _originalUID))
                                        return found; 
                                }
                            }
                        }
                    }
                    break;

                    case PropertyType::ObjectPtrVector:
                    {
                        if (const auto * objVec = prop->GetPropertyObjectPtrVector(_object))
                        {
                            for (const auto * obj : *objVec)
                            {
                                if (auto * found = findByOriginalUID((Object*)obj, _originalUID))
                                    return found;
                            }
                        }
                    }
                    break;
                }
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    const IObject * Object::FindByOriginalUID(UID _originalUID) const
    {
        return findByOriginalUID(this, _originalUID);
    }

    //--------------------------------------------------------------------------------------
    IObject * Object::Instanciate() const
    {
        return Kernel::getFactory()->Instanciate(this);
    }

    //--------------------------------------------------------------------------------------
    void Object::OnLoad()
    {
        auto selection = Kernel::getSelection();
        if (selection->IsSelectedObject(this))
            selection->Remove(this);    // Unselect object on load

        if (!HasValidUID())
            RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    void Object::OnSave()
    {
        auto selection = Kernel::getSelection();
        if (selection->IsSelectedObject(this))
            selection->Remove(this);    // Unselect object on save
    }

    //--------------------------------------------------------------------------------------
    void Object::OnPlay()
    {
   
    }

    //--------------------------------------------------------------------------------------
    void Object::OnStop()
    {
       
    }

    //--------------------------------------------------------------------------------------
    void Object::OnEnable()
    {

    }

    //--------------------------------------------------------------------------------------
    void Object::OnDisable()
    {

    }

    //--------------------------------------------------------------------------------------
    void Object::SetParent(IObject * _parent)
    {
        setParent(_parent);
    }

    //--------------------------------------------------------------------------------------
    IObject * Object::GetParent() const
    {
        return getParent();
    }

    //--------------------------------------------------------------------------------------
    void Object::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        if (_notifyParent && nullptr != m_parent)
            m_parent->OnPropertyChanged(_object, _prop, true);
    }

    //--------------------------------------------------------------------------------------
    void Object::SetPropertyValue(const IProperty & _prop, void * _previousValue, void * _newValue)
    {
        const auto propType = _prop.GetType();
        switch (propType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(propType);
                break;

        case PropertyType::Resource:
            ((Resource *)_previousValue)->SetResourcePath(((Resource *)_newValue)->GetResourcePath());
            break;

            case PropertyType::ObjectHandle:
                *(ObjectHandle *)_previousValue = *(ObjectHandle *)_newValue;
                break;

            case PropertyType::Bool:
                *(bool *)_previousValue = *(bool *)_newValue;
                break;

            case PropertyType::Int8:
            case PropertyType::EnumI8:
                *(i8 *)_previousValue = *(i8 *)_newValue;
                break;

            case PropertyType::Int16:
            case PropertyType::EnumI16:
                *(i16 *)_previousValue = *(i16 *)_newValue;
                break;

            case PropertyType::Int32:
            case PropertyType::EnumI32:
                *(i32 *)_previousValue = *(i32 *)_newValue;
                break;

            case PropertyType::Int64:
            case PropertyType::EnumI64:
                *(i64 *)_previousValue = *(i64 *)_newValue;
                break;

            case PropertyType::Int2:
                *(int2 *)_previousValue = *(int2 *)_newValue;
                break;

            case PropertyType::Int3:
                *(int3 *)_previousValue = *(int3 *)_newValue;
                break;

            case PropertyType::Int4:
                *(int4 *)_previousValue = *(int4 *)_newValue;
                break;

            case PropertyType::Uint8:
            case PropertyType::EnumU8:
            case PropertyType::EnumFlagsU8:
                *(u8 *)_previousValue = *(u8 *)_newValue;
                break;

            case PropertyType::Uint16:
            case PropertyType::EnumU16:
            case PropertyType::EnumFlagsU16:
                *(u16 *)_previousValue = *(u16 *)_newValue;
                break;

            case PropertyType::Uint32:
            case PropertyType::EnumU32:
            case PropertyType::EnumFlagsU32:
                *(u32 *)_previousValue = *(u32 *)_newValue;
                break;

            case PropertyType::Uint2:
                *(uint2 *)_previousValue = *(uint2 *)_newValue;
                break;

            case PropertyType::Uint3:
                *(uint3 *)_previousValue = *(uint3 *)_newValue;
                break;

            case PropertyType::Uint4:
                *(uint4 *)_previousValue = *(uint4 *)_newValue;
                break;

            case PropertyType::Uint64:
            case PropertyType::EnumU64:
            case PropertyType::EnumFlagsU64:
                *(u64 *)_previousValue = *(u64 *)_newValue;
                break;

            case PropertyType::Float:
                *(float *)_previousValue = *(float *)_newValue;
                break;

            case PropertyType::Float2:
                *(float2 *)_previousValue = *(float2 *)_newValue;
                break;

            case PropertyType::Float3:
                *(float3 *)_previousValue = *(float3 *)_newValue;
                break;

            case PropertyType::Float4:
                *(float4 *)_previousValue = *(float4 *)_newValue;
                break;

            case PropertyType::Float4x4:
                *(float4x4 *)_previousValue = *(float4x4 *)_newValue;
                break;

            case PropertyType::String:
                *(string *)_previousValue = *(string *)_newValue;
                break;
        }
    }

    //--------------------------------------------------------------------------------------
    void Object::OnResourceLoaded(IResource * _resource)
    {
        if (nullptr != m_parent)
            m_parent->OnResourceLoaded(_resource);
    }

    //--------------------------------------------------------------------------------------
    void Object::OnResourceUnloaded(IResource * _resource)
    {
        if (nullptr != m_parent)
            m_parent->OnResourceUnloaded(_resource);
    }

    //--------------------------------------------------------------------------------------
    void Object::SetClassDesc(const IClassDesc * _classDesc)
    {
        VG_ASSERT(nullptr == m_classDesc);
        m_classDesc = _classDesc;
    }

    //--------------------------------------------------------------------------------------
    bool Object::IsRegisteredClass() const
    {
        if (!m_classDesc)
        {
            const auto * factory = Kernel::getFactory();
            if (factory)
                return factory->IsRegisteredClass(GetClassName());
        }
        return true;
    }

    //--------------------------------------------------------------------------------------
    const IClassDesc * Object::GetClassDesc() const
    {
        if (!m_classDesc)
        {
            const auto * factory = Kernel::getFactory();
            if (factory)
                m_classDesc = factory->GetClassDescriptor(GetClassName());
        }
        VG_ASSERT(m_classDesc);
        return m_classDesc;
    }

	//--------------------------------------------------------------------------------------
	void Object::SetName(const string & _name)
	{
		m_name = _name;
	}

	//--------------------------------------------------------------------------------------
	const string & Object::GetName() const
	{
		return m_name;
	}

    //--------------------------------------------------------------------------------------
    const string Object::GetFullName() const
    {
        string name = GetName();
        IObject * parent = GetParent();
        while (parent)
        {
            // TODO: each class should override GetFullName and stop when != Scene or GameObject
            if (strcmp("Scene", parent->GetClassDesc()->GetClassName()))
            {
                name = parent->GetName() + ">" + name;
                parent = parent->GetParent();
            }
            else
            {
                parent = nullptr;
            }
        }
        return name;
    }

    //--------------------------------------------------------------------------------------
    IGameObject * Object::GetParentGameObject() const
    {
        const IObject * obj = this;
        while (obj)
        {
            if (obj->IsRegisteredClass())
            { 
                if (!strcmp("GameObject", obj->GetClassDesc()->GetClassName()))
                    return (IGameObject*)obj;
            }

            if (auto * parent = obj->GetParent())
            {
                VG_ASSERT(parent != this, "Object \"%s\" cannot have himself at its own parent!", obj->GetName().c_str());
                obj = parent;
            }
            else
            {
                break;
            }
        }
        return nullptr; 
    }

    //--------------------------------------------------------------------------------------
    IComponent * Object::GetParentComponent() const
    {
        const IObject * obj = this;
        while (obj)
        {
            if (obj->IsRegisteredClass())
            {
                if (!strcmp("Component", obj->GetClassDesc()->GetClassName()))
                    return (IComponent *)obj;
            }

            obj = obj->GetParent();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    const string Object::GetShortName() const
    {
        string name = GetName();
        const IObject * obj = this;
        while (obj)
        {
            // TODO: each class should override GetFullName and stop when != Scene or GameObject
            if (obj->IsRegisteredClass())
            {
                if (!strcmp("GameObject", obj->GetClassDesc()->GetClassName()))
                {
                    name = obj->GetName() + ">" + name;
                    return name;
                }
            }

            obj = obj->GetParent();
        }
        return name;
    }

    //--------------------------------------------------------------------------------------
    bool Object::HasFile() const
    {
        return !m_file.empty();
    }

    //--------------------------------------------------------------------------------------
    void Object::SetFile(const string & _file)
    {
        m_file = _file;
    }

    //--------------------------------------------------------------------------------------
    const string & Object::GetFile() const
    {
        return m_file;
    }

	//--------------------------------------------------------------------------------------
	u32 Object::AddRef()
	{
		return m_refCount.fetch_add(1) + 1;
	}

	//--------------------------------------------------------------------------------------
	u32 Object::Release()
	{
		const u32 prev = m_refCount.fetch_sub(1);
		if (1 == prev)
			delete this;
		return prev-1;
	}

    //--------------------------------------------------------------------------------------
    u32 Object::GetRefCount() const
    {
        return m_refCount.load();
    }

    //--------------------------------------------------------------------------------------
    bool Object::resizeVector(core::IObject * _parent, core::uint _offset, core::uint _count, void ** _data)
    {
        VG_ASSERT(false);
        return false;
    }	

    //--------------------------------------------------------------------------------------
    bool Object::CanOverrideProperties() const
    {
        return true;
    }
}