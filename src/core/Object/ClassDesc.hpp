#include "ClassDesc.h"
#include "core/IResource.h"
#include "core/Misc/BitMask/BitMask.h"

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
        
        VG_ASSERT("[Factory] Property \"%s\" not found in class '%s'", _propertyName, GetClassName());
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    uint ClassDesc::GetPropertyIndex(const char * _propertyName) const
    {
        for (uint i = 0; i < properties.size(); ++i)
        {
            const auto & prop = properties[i];
            if (!strcmp(_propertyName, prop.getName()))
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
    void ClassDesc::RegisterPropertyLayout(const char * _className, IProperty::LayoutElementType _layoutElementType, const char * _label, bool * _offset, IProperty::Flags _flags)
    {
        Property prop = Property(_className, _label, IProperty::Type::LayoutElement, (uint_ptr)_offset, (uint)_layoutElementType, _label, _flags);
        properties.push_back(prop);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::float2 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::float3 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::float4 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, float4x4 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::uint2 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::uint3 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::uint4 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::i8 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::i16 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::i32 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::u16 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::u32 * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, float * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::string * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, BitMask * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterPropertyResource(const char * _className, const char * _propertyName, core::IResource * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterPropertyResourcePtr(const char * _className, const char * _propertyName, core::IResource ** _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::IObject * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::IObject ** _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, IProperty::Callback _funcPtr, const char * _displayName, IProperty::Flags _flags)
    {
        properties.emplace_back(_className, _propertyName, IProperty::Type::Callback, (uint_ptr)_funcPtr, (u32)sizeof(void*), _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, core::u32 _sizeOf, void * _offset, const char * _displayName, IProperty::Flags _flags)
    {
        properties.emplace_back(_className, _propertyName, asBool(_flags & IProperty::Flags::Resource) ? IProperty::Type::ResourceVector : IProperty::Type::ObjectVector, (uint_ptr)_offset, _sizeOf, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, vector<IObject*>* _offset, const char * _elemType, const char * _displayName, IProperty::Flags _flags)
    {
        properties.emplace_back(_className, _propertyName, asBool(_flags & IProperty::Flags::Resource) ? IProperty::Type::ResourcePtrVector : IProperty::Type::ObjectPtrVector, (uint_ptr)_offset, (u32)sizeof(vector<IObject *>), _displayName, _flags);
        auto & prop = properties.back();
        prop.setInterface(_elemType);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterProperty(const char * _className, const char * _propertyName, dictionary<core::IObject*>* _offset, const char * _displayName, IProperty::Flags _flags)
    {
        registerClassMemberT(_className, _propertyName, _offset, _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u8 * _enumValues, IProperty::Flags _flags)
    {
        const bool bitfield = asBool(IProperty::Flags::Bitfield & _flags);
        properties.emplace_back(_className, _propertyName, bitfield ? IProperty::Type::EnumFlagsU8 : IProperty::Type::EnumU8, (uint_ptr)_offset, (u32)sizeof(u8), _displayName, _flags, _enumCount, _enumNames, _enumValues);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, core::u16 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u16 * _enumValues, IProperty::Flags _flags)
    {
        const bool bitfield = asBool(IProperty::Flags::Bitfield & _flags);
        properties.emplace_back(_className, _propertyName, bitfield ? IProperty::Type::EnumFlagsU16 : IProperty::Type::EnumU16, (uint_ptr)_offset, (u32)sizeof(u16), _displayName, _flags, _enumCount, _enumNames, _enumValues);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, core::u32 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u32 * _enumValues, IProperty::Flags _flags)
    {
        const bool bitfield = asBool(IProperty::Flags::Bitfield & _flags);
        properties.emplace_back(_className, _propertyName, bitfield ? IProperty::Type::EnumFlagsU32 : IProperty::Type::EnumU32, (uint_ptr)_offset, (u32)sizeof(u32), _displayName, _flags, _enumCount, _enumNames, _enumValues);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnum(const char * _className, const char * _propertyName, core::u64 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u64 * _enumValues, IProperty::Flags _flags)
    {
        const bool bitfield = asBool(IProperty::Flags::Bitfield & _flags);
        properties.emplace_back(_className, _propertyName, bitfield ? IProperty::Type::EnumFlagsU64 : IProperty::Type::EnumU64, (uint_ptr)_offset, (u32)sizeof(u64), _displayName, _flags, _enumCount, _enumNames, _enumValues);
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
    ClassDesc::Priority ClassDesc::GetPriority() const
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
    IClassDesc::Flags ClassDesc::GetFlags() const
    {
        return flags;
    }

    //--------------------------------------------------------------------------------------
    u32 ClassDesc::GetSizeOf() const
    {
        return sizeOf;
    }

    template <typename T> struct TypeToEnum;
    template <> struct TypeToEnum<bool> { static constexpr auto value = IProperty::Type::Bool; };
    template <> struct TypeToEnum<core::i8> { static constexpr auto value = IProperty::Type::Int8; };
    template <> struct TypeToEnum<core::i16> { static constexpr auto value = IProperty::Type::Int16; };
    template <> struct TypeToEnum<core::i32> { static constexpr auto value = IProperty::Type::Int32; };
    template <> struct TypeToEnum<core::i64> { static constexpr auto value = IProperty::Type::Int64; };
    template <> struct TypeToEnum<core::u8> { static constexpr auto value = IProperty::Type::Uint8; };
    template <> struct TypeToEnum<core::u16> { static constexpr auto value = IProperty::Type::Uint16; };
    template <> struct TypeToEnum<core::u32> { static constexpr auto value = IProperty::Type::Uint32; };
    template <> struct TypeToEnum<core::u64> { static constexpr auto value = IProperty::Type::Uint64; };
    template <> struct TypeToEnum<core::uint2> { static constexpr auto value = IProperty::Type::Uint2; };
    template <> struct TypeToEnum<core::uint3> { static constexpr auto value = IProperty::Type::Uint3; };
    template <> struct TypeToEnum<core::uint4> { static constexpr auto value = IProperty::Type::Uint4; };
    template <> struct TypeToEnum<float> { static constexpr auto value = IProperty::Type::Float; };
    template <> struct TypeToEnum<core::float2> { static constexpr auto value = IProperty::Type::Float2; };
    template <> struct TypeToEnum<core::float3> { static constexpr auto value = IProperty::Type::Float3; };
    template <> struct TypeToEnum<core::float4> { static constexpr auto value = IProperty::Type::Float4; };
    template <> struct TypeToEnum<core::float4x4> { static constexpr auto value = IProperty::Type::Float4x4; };
    template <> struct TypeToEnum<core::string> { static constexpr auto value = IProperty::Type::String; };
    template <> struct TypeToEnum<core::BitMask> { static constexpr auto value = IProperty::Type::BitMask; };
    template <> struct TypeToEnum<IObject> { static constexpr auto value = IProperty::Type::Object; };
    template <> struct TypeToEnum<IObject*> { static constexpr auto value = IProperty::Type::ObjectPtr; };
    template <> struct TypeToEnum<IResource> { static constexpr auto value = IProperty::Type::Resource; };
    template <> struct TypeToEnum<IResource*> { static constexpr auto value = IProperty::Type::ResourcePtr; };
    template <> struct TypeToEnum<IProperty::Callback> { static constexpr auto value = IProperty::Type::Callback; };
    template <> struct TypeToEnum<vector<IObject*>> { static constexpr auto value = IProperty::Type::ObjectPtrVector; };
    template <> struct TypeToEnum<dictionary<core::IObject*>> { static constexpr auto value = IProperty::Type::ObjectPtrDictionary; };

    //--------------------------------------------------------------------------------------
    template <typename T> void ClassDesc::registerClassMemberT(const char * _className, const char * _propertyName, T * _offset, const char * _displayName, IProperty::Flags _flags)
    {
#pragma warning(push)
#pragma warning (disable: 4302)
#pragma warning (disable: 4311)
        const u32 offset = (u32)(_offset);
#pragma warning( pop )

        properties.emplace_back(_className, _propertyName, TypeToEnum<T>::value, offset, (u32)sizeof(T), _displayName, _flags);
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void ClassDesc::registerEnumArrayT(const char * _className, const char * _propertyName, T * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf)
    {
        VG_ASSERT(!asBool(IProperty::Flags::Bitfield & _flags));
        auto type = TypeToEnum<T>::value;
        
        if (asBool(IProperty::Flags::Resource & _flags))
            VG_ASSERT(type == IProperty::Type::Resource);
        //else
        //    VG_ASSERT(type == IProperty::Type::Object);

        properties.emplace_back(_className, _propertyName, type, (uint_ptr)_offset, _elementSize, _displayName, IProperty::Flags::EnumArray | _flags, _enumCount, _enumNames, _enumValues, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, core::float4 * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterEnumArray(const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf)
    {
        registerEnumArrayT(_className, _propertyName, _offset, _displayName, _enumCount, _elementSize, _enumNames, _enumValues, _flags, _enumSizeOf);
    }

    //--------------------------------------------------------------------------------------
    void ClassDesc::RegisterResizeVectorFunc(const char * _className, IClassDesc::ResizeVectorFunc _func)
    {
        resizeVectorFunc = _func;
    }

    //--------------------------------------------------------------------------------------
    void * ClassDesc::ResizeVector(core::IObject * _parent, core::uint _offset, core::uint _count, core::uint & _elementSize) const
    {
        if (nullptr != resizeVectorFunc)
            return resizeVectorFunc(_parent, _offset, _count, _elementSize);
        
        VG_ASSERT(nullptr != resizeVectorFunc, "Class \"%s\" has no 'ResizeVector' callback registered", name);
        return nullptr;
    }
}