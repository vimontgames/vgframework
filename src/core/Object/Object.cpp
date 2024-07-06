#include "core/Precomp.h"
#include "Object.h"
#include "core/Kernel.h"
#include "core/File/File.h"

#if !VG_ENABLE_INLINE
#include "Object.inl"
#endif

#include "Property.hpp"
#include "ClassDesc.hpp"
#include "AutoRegisterClass.hpp"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool Object::registerProperties(IClassDesc & _desc)
    {
        registerPropertyEx(Object, m_uid, "UID", IProperty::Flags::Debug | IProperty::Flags::Hexadecimal);
        registerPropertyEx(Object, m_name, "Name", IProperty::Flags::NotVisible);
        registerPropertyEnumBitfieldEx(Object, ObjectFlags,m_objectFlags, "Object Flags", IProperty::Flags::Debug);

        return true;
    }

	//--------------------------------------------------------------------------------------
	Object::Object(const core::string & _name, IObject * _parent) :
		m_refCount(1)
	{
		setName(_name);
        setParent(_parent);
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

	}

    //--------------------------------------------------------------------------------------
    bool Object::FixMissingUID()
    {
        if (!HasValidUID())
        {
            IFactory * factory = Kernel::getFactory();
            SetUID(factory->CreateNewUID(this));
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
    UID Object::GetUID() const
    {
        VG_ASSERT((UID)0 != m_uid, "Object \"%s\" has no UID", getName().c_str());
        return m_uid;
    }

    //--------------------------------------------------------------------------------------
    void Object::SetUID(UID _uid)
    {
        IFactory * factory = Kernel::getFactory();
        if (m_uid)
            factory->ReleaseUID(m_uid);
        m_uid = _uid;
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
    IObject * Object::Instanciate() const
    {
        return Kernel::getFactory()->Instanciate(this);
    }

    //--------------------------------------------------------------------------------------
    void Object::OnLoad()
    {
        FixMissingUID();
    }

    //--------------------------------------------------------------------------------------
    void Object::OnPlay()
    {
        auto * factory = Kernel::getFactory();
        if (factory)
            factory->SaveProperties(this);
    }

    //--------------------------------------------------------------------------------------
    void Object::OnStop()
    {
        auto * factory = Kernel::getFactory();
        if (factory)
            factory->RestoreProperties(this);        
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
    void Object::setParent(IObject * _parent)
    {
        m_parent = (Object*)_parent;
    }

    //--------------------------------------------------------------------------------------
    IObject * Object::getParent() const
    {
        return (IObject*)m_parent;
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
        const auto propType = _prop.getType();
        switch (propType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(propType);
                break;

            case IProperty::Type::Bool:
                *(bool *)_previousValue = *(bool *)_newValue;
                break;

            case IProperty::Type::Int8:
                *(i8 *)_previousValue = *(i8 *)_newValue;
                break;

            case IProperty::Type::Int16:
                *(i16 *)_previousValue = *(i16 *)_newValue;
                break;

            case IProperty::Type::Int32:
                *(i32 *)_previousValue = *(i32 *)_newValue;
                break;

            case IProperty::Type::Int64:
                *(i64 *)_previousValue = *(i64 *)_newValue;
                break;

            case IProperty::Type::Uint8:
            case IProperty::Type::EnumU8:
            case IProperty::Type::EnumFlagsU8:
                *(u8 *)_previousValue = *(u8 *)_newValue;
                break;

            case IProperty::Type::Uint16:
            case IProperty::Type::EnumU16:
            case IProperty::Type::EnumFlagsU16:
                *(u16 *)_previousValue = *(u16 *)_newValue;
                break;

            case IProperty::Type::Uint32:
            case IProperty::Type::EnumU32:
            case IProperty::Type::EnumFlagsU32:
                *(u32 *)_previousValue = *(u32 *)_newValue;
                break;

            case IProperty::Type::Uint2:
                *(uint2 *)_previousValue = *(uint2 *)_newValue;
                break;

            case IProperty::Type::Uint3:
                *(uint3 *)_previousValue = *(uint3 *)_newValue;
                break;

            case IProperty::Type::Uint4:
                *(uint4 *)_previousValue = *(uint4 *)_newValue;
                break;

            case IProperty::Type::Uint64:
            case IProperty::Type::EnumU64:
            case IProperty::Type::EnumFlagsU64:
                *(u64 *)_previousValue = *(u64 *)_newValue;
                break;

            case IProperty::Type::Float:
                *(float *)_previousValue = *(float *)_newValue;
                break;

            case IProperty::Type::Float2:
                *(float2 *)_previousValue = *(float2 *)_newValue;
                break;

            case IProperty::Type::Float3:
                *(float3 *)_previousValue = *(float3 *)_newValue;
                break;

            case IProperty::Type::Float4:
                *(float4 *)_previousValue = *(float4 *)_newValue;
                break;

            case IProperty::Type::Float4x4:
                *(float4x4 *)_previousValue = *(float4x4 *)_newValue;
                break;

            case IProperty::Type::String:
                *(string *)_previousValue = *(string *)_newValue;
                break;
        }
    }

    //--------------------------------------------------------------------------------------
    void Object::onResourceLoaded(IResource * _resource)
    {
        if (nullptr != m_parent)
            m_parent->onResourceLoaded(_resource);
    }

    //--------------------------------------------------------------------------------------
    void Object::onResourceUnloaded(IResource * _resource)
    {
        if (nullptr != m_parent)
            m_parent->onResourceUnloaded(_resource);
    }

    //--------------------------------------------------------------------------------------
    void  Object::SetClassDesc(const IClassDesc * _classDesc)
    {
        VG_ASSERT(nullptr == m_classDesc);
        m_classDesc = _classDesc;
    }

    //--------------------------------------------------------------------------------------
    const IClassDesc * Object::GetClassDesc() const
    {
        if (!m_classDesc)
        {
            const auto * factory = Kernel::getFactory();
            if (factory)
                m_classDesc = factory->getClassDescriptor(GetClassName());
        }
        VG_ASSERT(m_classDesc);
        return m_classDesc;
    }

	//--------------------------------------------------------------------------------------
	void Object::setName(const string & _name)
	{
		m_name = _name;
	}

	//--------------------------------------------------------------------------------------
	const string & Object::getName() const
	{
		return m_name;
	}

    //--------------------------------------------------------------------------------------
    bool Object::hasFile() const
    {
        return !m_file.empty();
    }

    //--------------------------------------------------------------------------------------
    void Object::setFile(const string & _file)
    {
        m_file = _file;
    }

    //--------------------------------------------------------------------------------------
    const string & Object::getFile() const
    {
        return m_file;
    }

	//--------------------------------------------------------------------------------------
	u32 Object::addRef()
	{
		return m_refCount.fetch_add(1) + 1;
	}

	//--------------------------------------------------------------------------------------
	u32 Object::release()
	{
		const u32 prev = m_refCount.fetch_sub(1);
		if (1 == prev)
			delete this;
		return prev-1;
	}

    //--------------------------------------------------------------------------------------
    u32 Object::getRefCount() const
    {
        return m_refCount.load();
    }

	//--------------------------------------------------------------------------------------
	u32 Object::Release()
	{
		return release();
	}

    //--------------------------------------------------------------------------------------
    bool Object::resizeVector(core::IObject * _parent, core::uint _offset, core::uint _count, void ** _data)
    {
        VG_ASSERT(false);
        return false;
    }
}