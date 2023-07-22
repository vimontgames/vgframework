#include "Property.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Property::Property(const char * _class, const char * _name, Type _type, uint_ptr _offset, core::u32 _sizeOf, const char * _prettyName, Flags _flags, uint _enumCount, const char * _enumNames, const void * _enumValues) :
        className(_class),
        name(_name),
        type(_type),
        offset(_offset),
        sizeOf(_sizeOf),
        displayName(_prettyName),
        flags(_flags)
    {
        if (_enumCount > 0)
        {
            switch (_type)
            {
                case Type::EnumU8:
                case Type::EnumFlagsU8:
                    initEnum<u8>(_enumCount, _enumNames, _enumValues);
                    break;
                case Type::EnumU16:
                case Type::EnumFlagsU16:
                    initEnum<u16>(_enumCount, _enumNames, _enumValues);
                    break;
                case Type::EnumU32:
                case Type::EnumFlagsU32:
                    initEnum<u32>(_enumCount, _enumNames, _enumValues);
                    break;
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_type);
                    break;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void Property::initEnum(uint _enumCount, const char * _enumNames, const void * _enumValues)
    {
        enums.resize(_enumCount);
        const char * name = _enumNames;
        for (uint e = 0; e < _enumCount; ++e)
        {
            char temp[256] = { '\0' };
            char * dst = temp;
            while (*name != '\0')
                *dst++ = *name++;
            *dst = '\0';
            ++name;
            enums[e].name = temp;
            enums[e].value = ((T*)_enumValues)[e];
        }
    }

    //--------------------------------------------------------------------------------------
    Property::~Property()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void Property::setRange(float2 _range)
    {
        if ((float)_range.x != (float)_range.y)
            flags |= IProperty::Flags::HasRange;

        range = _range;
    }

    //--------------------------------------------------------------------------------------
    void Property::setFlags(Flags _flagsToSet, Flags _flagsToRemove)
    {
        flags = (Flags)((std::underlying_type<Flags>::type(flags) & ~std::underlying_type<Flags>::type(_flagsToRemove)) | (std::underlying_type<Flags>::type(_flagsToSet)));
    }

    //--------------------------------------------------------------------------------------
    u32 Property::getEnumCount() const
    {
        return enums.count();
    }

    //--------------------------------------------------------------------------------------
    const char * Property::getEnumName(uint index) const
    {
        return enums[index].name.c_str();
    }

    //--------------------------------------------------------------------------------------
    u32 Property::getEnumValue(uint index) const
    {
        return enums[index].value;
    }

    //--------------------------------------------------------------------------------------
    float2 Property::getRange() const
    {
        return range;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::getName() const
    {
        return name;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::getClassName() const
    {
        return className;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::getDisplayName() const
    {
        if (displayName)
            return displayName;
        else
            return name;
    }

    //--------------------------------------------------------------------------------------
    IProperty::Type Property::getType() const
    {
        return type;
    }

    //--------------------------------------------------------------------------------------
    IProperty::Flags Property::getFlags() const
    {
        return flags;
    }

    //--------------------------------------------------------------------------------------
    uint_ptr Property::getOffset() const
    {
        return offset;
    }

    //--------------------------------------------------------------------------------------
    core::u32 Property::getSizeOf() const
    {
        return sizeOf;
    }

    //--------------------------------------------------------------------------------------
    bool * Property::GetPropertyBool(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Bool == getType());
        return (bool*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::u8 * Property::GetPropertyUint8(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Uint8 == getType() || Type::EnumU8 == getType() || Type::EnumFlagsU8 == getType());
        return (u8*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::u16 * Property::GetPropertyUint16(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Uint16 == getType() || Type::EnumU16 == getType() || Type::EnumFlagsU16 == getType());
        return (u16*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::u32 * Property::GetPropertyUint32(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Uint32 == getType() || Type::EnumU32 == getType() || Type::EnumFlagsU32 == getType());
        return (u32*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::u64 * Property::GetPropertyUint64(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Uint64 == getType() || Type::EnumU64 == getType() || Type::EnumFlagsU64 == getType());
        return (u64 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    float * Property::GetPropertyFloat(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Float == getType());
        return (float*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::float4 * Property::GetPropertyFloat4(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Float4 == getType());
        return (float4*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::float4x4 * Property::GetPropertyFloat4x4(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Float4x4 == getType());
        return (float4x4*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::string * Property::GetPropertyString(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::String == getType());
        return (string*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    IResource * Property::GetPropertyResource(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Resource == getType());
        return (IResource*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    IObject * Property::GetPropertyObjectRef(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::ObjectRef == getType());
        return *(IObject**)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    vector<IObject*> * Property::GetPropertyObjectRefVector(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::ObjectRefVector == getType());
        return (vector<IObject*>*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    dictionary<IObject*> * Property::GetPropertyObjectRefDictionary(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::ObjectRefDictionary == getType());
        return (dictionary<IObject*>*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    uint Property::GetPropertyObjectVectorCount(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::ObjectVector == getType());
        return (uint)(((vector<u8>*)(uint_ptr(_object) + offset))->end() - ((vector<u8>*)(uint_ptr(_object) + offset))->begin()) / getSizeOf();
    }

    //--------------------------------------------------------------------------------------
    u8 * Property::GetPropertyObjectVectorData(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::ObjectVector == getType());
        return ((vector<u8>*)(uint_ptr(_object) + offset))->data();
    }

    //--------------------------------------------------------------------------------------
    uint Property::GetPropertyResourceVectorCount(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::ResourceVector == getType());
        return (uint)(((vector<u8>*)(uint_ptr(_object) + offset))->end() - ((vector<u8>*)(uint_ptr(_object) + offset))->begin()) / getSizeOf();
    }

    //--------------------------------------------------------------------------------------
    u8 * Property::GetPropertyResourceVectorData(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::ResourceVector == getType());
        return ((vector<u8>*)(uint_ptr(_object) + offset))->data();
    }

    //--------------------------------------------------------------------------------------
    IProperty::Callback Property::GetPropertyCallback() const
    {
        VG_ASSERT(Type::Callback == getType());
        return (IProperty::Callback)offset;
    }
}