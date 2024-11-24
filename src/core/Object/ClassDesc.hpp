#include "ClassDesc.h"
#include "core/IResource.h"
#include "core/Misc/BitMask/BitMask.h"
#include "core/Object/ObjectHandle.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    ClassDesc::ClassDesc()
    {

    }

    //--------------------------------------------------------------------------------------
    ClassDesc::~ClassDesc()
    {

    }

    //--------------------------------------------------------------------------------------
    u32 ClassDesc::GetNextIndex() const
    {
        return count++;
    }

    //--------------------------------------------------------------------------------------
    bool ClassDesc::IsRegisteredProperty(const char * _propertyName)
    {
        return nullptr != GetPropertyByName(_propertyName);
    }

    //--------------------------------------------------------------------------------------
    IProperty * ClassDesc::GetPropertyByName(const char * _propertyName) const
    {
        uint index = GetPropertyIndex(_propertyName);
        if (-1 != index)
            return (IProperty*)&properties[index];
        
        VG_WARNING("[Factory] Property \"%s\" not found in class '%s'", _propertyName, GetClassName());
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    uint ClassDesc::GetPropertyIndex(const char * _propertyName) const
    {
        for (uint i = 0; i < properties.size(); ++i)
        {
            const auto & prop = properties[i];
            if (!strcmp(_propertyName, prop.GetName()))
                return i;
        }
        return -1;
    }

    //--------------------------------------------------------------------------------------
    IProperty * ClassDesc::GetPreviousProperty(const char * _propertyName) const
    {
        uint index = GetPropertyIndex(_propertyName);
        if (index > 0)
            return  (IProperty *)&properties[index-1];
        
        VG_ASSERT("[Factory] Previous Property of \"%s\" not found in class '%s'", _propertyName, GetClassName());
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    IProperty * ClassDesc::GetNextProperty(const char * _propertyName) const
    {
        uint index = GetPropertyIndex(_propertyName);
        if (index < properties.size() - 1 )
            return  (IProperty *)&properties[index + 1];

        VG_ASSERT("[Factory] Next Property of \"%s\" not found in class '%s'", _propertyName, GetClassName());
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool ClassDesc::pushProperty(const Property & _prop)
    {
        for (auto & prop : properties)
        {
            const bool isAlreadyUsed = _prop.GetName()[0] != '\0' && !strcmp(_prop.GetName(), prop.GetName());

            if (isAlreadyUsed)
            {
                VG_ASSERT(!isAlreadyUsed, "[Factory] Property \"%s\" from \"%s\" is already declared in \"%s\"", _prop.GetName(), _prop.GetClassName(), prop.GetClassName());
                return false;
            }
        }

        properties.push_back(_prop);
        return true;
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterPropertyLayout(const char * _className, PropertyLayoutElement _layoutElementType, const char * _label, bool * _offset, PropertyFlags _flags)
    {
        Property prop = Property(_className, _label, PropertyType::LayoutElement, (uint_ptr)_offset, (uint)_layoutElementType, _label, _flags);
        pushProperty(prop);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::float2 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::float3 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::float4 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, float4x4 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::uint2 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::uint3 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::uint4 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::int2 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::int3 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::int4 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::i8 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::i16 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::i32 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::i64 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::u16 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::u32 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::u64 * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, float * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::string * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, BitMask * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, ObjectHandle * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterPropertyResource(const char * _className, const char * _propertyName, core::IResource * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterPropertyResourcePtr(const char * _className, const char * _propertyName, core::IResource ** _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::IObject * _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::IObject ** _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, IProperty::ActionCallback _funcPtr, const char * _displayName, PropertyFlags _flags)
    {
        pushProperty({ _className, _propertyName, PropertyType::Callback, (uint_ptr)_funcPtr, (u32)sizeof(void *), _displayName, _flags });
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::u32 _sizeOf, void * _offset, const char * _displayName, PropertyFlags _flags)
    {
        pushProperty({ _className, _propertyName, asBool(_flags & PropertyFlags::Resource) ? PropertyType::ResourceVector : PropertyType::ObjectVector, (uint_ptr)_offset, _sizeOf, _displayName, _flags });
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, vector<IObject*>* _offset, const char * _elemType, const char * _displayName, PropertyFlags _flags)
    {
        pushProperty({ _className, _propertyName, asBool(_flags & PropertyFlags::Resource) ? PropertyType::ResourcePtrVector : PropertyType::ObjectPtrVector, (uint_ptr)_offset, (u32)sizeof(vector<IObject *>), _displayName, _flags });
        auto & prop = properties.back();
        prop.SetInterface(_elemType);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, dictionary<core::IObject*>* _offset, const char * _displayName, PropertyFlags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, const char * _enumTypeName, core::u8 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u8 * _enumValues, PropertyFlags _flags)
    {
        const bool bitfield = asBool(PropertyFlags::Bitfield & _flags);
        pushProperty({ _className, _propertyName, bitfield ? PropertyType::EnumFlagsU8 : PropertyType::EnumU8, (uint_ptr)_offset, (u32)sizeof(u8), _displayName, _flags, _enumCount, _enumNames, _enumValues });
        auto & prop = properties.back();
        prop.SetEnumTypeName(_enumTypeName);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, const char * _enumTypeName, core::u16 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u16 * _enumValues, PropertyFlags _flags)
    {
        const bool bitfield = asBool(PropertyFlags::Bitfield & _flags);
        pushProperty({ _className, _propertyName, bitfield ? PropertyType::EnumFlagsU16 : PropertyType::EnumU16, (uint_ptr)_offset, (u32)sizeof(u16), _displayName, _flags, _enumCount, _enumNames, _enumValues });
        auto & prop = properties.back();
        prop.SetEnumTypeName(_enumTypeName);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, const char * _enumTypeName, core::u32 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u32 * _enumValues, PropertyFlags _flags)
    {
        const bool bitfield = asBool(PropertyFlags::Bitfield & _flags);
        pushProperty({ _className, _propertyName, bitfield ? PropertyType::EnumFlagsU32 : PropertyType::EnumU32, (uint_ptr)_offset, (u32)sizeof(u32), _displayName, _flags, _enumCount, _enumNames, _enumValues });
        auto & prop = properties.back();
        prop.SetEnumTypeName(_enumTypeName);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, const char * _enumTypeName, core::u64 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u64 * _enumValues, PropertyFlags _flags)
    {
        const bool bitfield = asBool(PropertyFlags::Bitfield & _flags);
        pushProperty({ _className, _propertyName, bitfield ? PropertyType::EnumFlagsU64 : PropertyType::EnumU64, (uint_ptr)_offset, (u32)sizeof(u64), _displayName, _flags, _enumCount, _enumNames, _enumValues });
        auto & prop = properties.back();
        prop.SetEnumTypeName(_enumTypeName);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, const char * _enumTypeName, core::i8 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const i8 * _enumValues, PropertyFlags _flags)
    {
        const bool bitfield = asBool(PropertyFlags::Bitfield & _flags);
        pushProperty({ _className, _propertyName, bitfield ? PropertyType::EnumFlagsI8 : PropertyType::EnumI8, (uint_ptr)_offset, (u32)sizeof(i8), _displayName, _flags, _enumCount, _enumNames, _enumValues });
        auto & prop = properties.back();
        prop.SetEnumTypeName(_enumTypeName);
    }
    
    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, const char * _enumTypeName, core::i16 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const i16 * _enumValues, PropertyFlags _flags)
    {
        const bool bitfield = asBool(PropertyFlags::Bitfield & _flags);
        pushProperty({ _className, _propertyName, bitfield ? PropertyType::EnumFlagsI16 : PropertyType::EnumI16, (uint_ptr)_offset, (u32)sizeof(i16), _displayName, _flags, _enumCount, _enumNames, _enumValues });
        auto & prop = properties.back();
        prop.SetEnumTypeName(_enumTypeName);
    }
    
    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, const char * _enumTypeName, core::i32 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const i32 * _enumValues, PropertyFlags _flags)
    {
        const bool bitfield = asBool(PropertyFlags::Bitfield & _flags);
        pushProperty({ _className, _propertyName, bitfield ? PropertyType::EnumFlagsI32 : PropertyType::EnumI32, (uint_ptr)_offset, (u32)sizeof(i32), _displayName, _flags, _enumCount, _enumNames, _enumValues });
        auto & prop = properties.back();
        prop.SetEnumTypeName(_enumTypeName);
    }
    
    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, const char * _enumTypeName, core::i64 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const i64 * _enumValues, PropertyFlags _flags)
    {
        const bool bitfield = asBool(PropertyFlags::Bitfield & _flags);
        pushProperty({ _className, _propertyName, bitfield ? PropertyType::EnumFlagsI64 : PropertyType::EnumI64, (uint_ptr)_offset, (u32)sizeof(i64), _displayName, _flags, _enumCount, _enumNames, _enumValues });
        auto & prop = properties.back();
        prop.SetEnumTypeName(_enumTypeName);
    }

    //--------------------------------------------------------------------------------------
    const char * ClassDesc::GetClassName() const
    {
        return name;
    }

    //--------------------------------------------------------------------------------------
    const char * ClassDesc::GetParentClassName() const
    {
        return parentClassName;
    }

    //--------------------------------------------------------------------------------------
    const char * ClassDesc::GetClassDisplayName() const
    {
        return displayName;
    }

    //--------------------------------------------------------------------------------------
    const char * ClassDesc::GetCategory() const
    {
        return category;
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::SetCategory(const char * _category)
    {
        category = _category;
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::SetDescription(const char * _description)
    {
        description = _description;
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::SetIcon(const char * _icon)
    {
        icon = _icon;
    }

    //--------------------------------------------------------------------------------------
    const char * ClassDesc::GetDescription() const
    {
        return description;
    }

    //--------------------------------------------------------------------------------------
    const char * ClassDesc::GetIcon() const
    {
        return icon;
    }

    //--------------------------------------------------------------------------------------
    Priority ClassDesc::GetPriority() const
    {
        return priority;
    }
    
    //--------------------------------------------------------------------------------------
    void ClassDesc::SetPriority(Priority _priority)
    {
        priority = _priority;
    }

    //--------------------------------------------------------------------------------------
    uint ClassDesc::GetPropertyCount() const
    {
        return (uint)properties.size();
    }

    //--------------------------------------------------------------------------------------
    const IProperty * ClassDesc::GetPropertyByIndex(uint _index) const
    {
        return &properties[_index];
    }

    //--------------------------------------------------------------------------------------
    IClassDesc::Func ClassDesc::GetCreateFunc() const
    {
        return createFunc;
    }

    //--------------------------------------------------------------------------------------
    IClassDesc::SingletonFunc ClassDesc::GetSingletonFunc() const
    {
        return createSingletonFunc;
    }

    //--------------------------------------------------------------------------------------
    ClassDescFlags ClassDesc::GetFlags() const
    {
        return flags;
    }

    //--------------------------------------------------------------------------------------
    u32 ClassDesc::GetSizeOf() const
    {
        return sizeOf;
    }

    template <typename T> struct TypeToEnum;
    template <> struct TypeToEnum<bool> { static constexpr auto value = PropertyType::Bool; };
    template <> struct TypeToEnum<core::i8> { static constexpr auto value = PropertyType::Int8; };
    template <> struct TypeToEnum<core::i16> { static constexpr auto value = PropertyType::Int16; };
    template <> struct TypeToEnum<core::i32> { static constexpr auto value = PropertyType::Int32; };
    template <> struct TypeToEnum<core::i64> { static constexpr auto value = PropertyType::Int64; };
    template <> struct TypeToEnum<core::u8> { static constexpr auto value = PropertyType::Uint8; };
    template <> struct TypeToEnum<core::u16> { static constexpr auto value = PropertyType::Uint16; };
    template <> struct TypeToEnum<core::u32> { static constexpr auto value = PropertyType::Uint32; };
    template <> struct TypeToEnum<core::u64> { static constexpr auto value = PropertyType::Uint64; };
    template <> struct TypeToEnum<core::uint2> { static constexpr auto value = PropertyType::Uint2; };
    template <> struct TypeToEnum<core::uint3> { static constexpr auto value = PropertyType::Uint3; };
    template <> struct TypeToEnum<core::uint4> { static constexpr auto value = PropertyType::Uint4; };
    template <> struct TypeToEnum<core::int2> { static constexpr auto value = PropertyType::Int2; };
    template <> struct TypeToEnum<core::int3> { static constexpr auto value = PropertyType::Int3; };
    template <> struct TypeToEnum<core::int4> { static constexpr auto value = PropertyType::Int4; };
    template <> struct TypeToEnum<float> { static constexpr auto value = PropertyType::Float; };
    template <> struct TypeToEnum<core::float2> { static constexpr auto value = PropertyType::Float2; };
    template <> struct TypeToEnum<core::float3> { static constexpr auto value = PropertyType::Float3; };
    template <> struct TypeToEnum<core::float4> { static constexpr auto value = PropertyType::Float4; };
    template <> struct TypeToEnum<core::float4x4> { static constexpr auto value = PropertyType::Float4x4; };
    template <> struct TypeToEnum<core::string> { static constexpr auto value = PropertyType::String; };
    template <> struct TypeToEnum<core::BitMask> { static constexpr auto value = PropertyType::BitMask; };
    template <> struct TypeToEnum<core::ObjectHandle> { static constexpr auto value = PropertyType::ObjectHandle; };
    template <> struct TypeToEnum<IObject> { static constexpr auto value = PropertyType::Object; };
    template <> struct TypeToEnum<IObject*> { static constexpr auto value = PropertyType::ObjectPtr; };
    template <> struct TypeToEnum<IResource> { static constexpr auto value = PropertyType::Resource; };
    template <> struct TypeToEnum<IResource*> { static constexpr auto value = PropertyType::ResourcePtr; };
    template <> struct TypeToEnum<IProperty::ActionCallback> { static constexpr auto value = PropertyType::Callback; };
    template <> struct TypeToEnum<vector<IObject*>> { static constexpr auto value = PropertyType::ObjectPtrVector; };
    template <> struct TypeToEnum<dictionary<core::IObject*>> { static constexpr auto value = PropertyType::ObjectPtrDictionary; };

    //--------------------------------------------------------------------------------------
    template <typename T> void ClassDesc::registerClassMemberT(const char * _className, const char * _propertyName, T * _offset, const char * _displayName, PropertyFlags _flags)
    {
#pragma warning(push)
#pragma warning (disable: 4302)
#pragma warning (disable: 4311)
        const u32 offset = (u32)(_offset);
#pragma warning( pop )

        pushProperty({ _className, _propertyName, TypeToEnum<T>::value, offset, (u32)sizeof(T), _displayName, _flags });
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void ClassDesc::registerEnumArrayT(const char * _className, const char * _propertyName, T * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf)
    {
        VG_ASSERT(!asBool(PropertyFlags::Bitfield & _flags));
        auto type = TypeToEnum<T>::value;
        
        if (asBool(PropertyFlags::Resource & _flags))
            VG_ASSERT(type == PropertyType::Resource);
        //else
        //    VG_ASSERT(type == PropertyType::Object);

        pushProperty({ _className, _propertyName, type, (uint_ptr)_offset, _elementSize, _displayName, PropertyFlags::EnumArray | _flags, _enumCount, _enumNames, _enumValues, _enumSizeOf });
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, core::string * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, core::float4 * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterResizeVectorFunc(const char * _className, IClassDesc::ResizeVectorFunc _func)
    {
        resizeVectorFunc = _func;
    }

    //--------------------------------------------------------------------------------------
    void * ClassDesc::ResizeVector(core::IObject * _parent, core::uint _offset, core::uint _count) const
    {
        if (nullptr != resizeVectorFunc)
            return resizeVectorFunc(_parent, _offset, _count);
        
        VG_ASSERT(nullptr != resizeVectorFunc, "Class \"%s\" has no 'ResizeVector' callback registered", name);
        return nullptr;
    }
}