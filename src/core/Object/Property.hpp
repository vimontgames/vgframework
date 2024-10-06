#include "Property.h"
#include "core/Types/Traits.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Property::Property(const char * _class, const char * _name, PropertyType _type, uint_ptr _offset, core::u32 _sizeOf, const char * _prettyName, PropertyFlags _flags, uint _enumCount, const char * _enumNames, const void * _enumValues, uint _enumSizeOf) :
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
                default:
                    VG_ASSERT(_enumCount == 0, "Property of Type \"%s\" does not support Enum/EnumArray", asString(_type).c_str());
                    break;

                case PropertyType::EnumU8:
                case PropertyType::EnumFlagsU8:
                    initEnum<u8>(_enumCount, _enumNames, _enumValues);
                    break;
                case PropertyType::EnumU16:
                case PropertyType::EnumFlagsU16:
                    initEnum<u16>(_enumCount, _enumNames, _enumValues);
                    break;
                case PropertyType::EnumU32:
                case PropertyType::EnumFlagsU32:
                    initEnum<u32>(_enumCount, _enumNames, _enumValues);
                    break;
                case PropertyType::EnumU64:
                case PropertyType::EnumFlagsU64:
                    initEnum<u64>(_enumCount, _enumNames, _enumValues);
                    break;

                case PropertyType::EnumI8:
                case PropertyType::EnumFlagsI8:
                    initEnum<i8>(_enumCount, _enumNames, _enumValues);
                    break;
                case PropertyType::EnumI16:
                case PropertyType::EnumFlagsI16:
                    initEnum<i16>(_enumCount, _enumNames, _enumValues);
                    break;
                case PropertyType::EnumI32:
                case PropertyType::EnumFlagsI32:
                    initEnum<i32>(_enumCount, _enumNames, _enumValues);
                    break;
                case PropertyType::EnumI64:
                case PropertyType::EnumFlagsI64:
                    initEnum<i64>(_enumCount, _enumNames, _enumValues);
                    break;

                // Types that support enumArray
                case PropertyType::Resource:
                case PropertyType::String:
                case PropertyType::Uint8:
                case PropertyType::Float4:
                    if (asBool(PropertyFlags::EnumArray & _flags))
                    {
                        switch (_enumSizeOf)
                        {
                            default:
                                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_type);
                                break;

                            case 1:
                                initEnum<u8>(_enumCount, _enumNames, _enumValues);
                                break;
                            case 2:
                                initEnum<u16>(_enumCount, _enumNames, _enumValues);
                                break;
                            case 4:
                                initEnum<u32>(_enumCount, _enumNames, _enumValues);
                                break;
                            case 8:
                                initEnum<u64>(_enumCount, _enumNames, _enumValues);
                                break;
                        }
                    }
                    else
                    {
                        VG_ASSERT(false, "[Factory] EnumArray not implemented for type \"%s\"", _type);
                    }
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

            if (scalarTraits<T>::is_signed)
                enums[e].value.s = ((T*)_enumValues)[e];
            else
                enums[e].value.u = ((T*)_enumValues)[e];
        }
    }

    //--------------------------------------------------------------------------------------
    bool Property::SetEnumValueFlags(u64 _value, EnumValueFlags _flags, bool _enabled)
    {
        for (uint i = 0; i < GetEnumCount(); ++i)
        {
            if (_value == GetUnsignedEnumValue(i))
            {
                EnumDesc & desc = enums[i];
                auto current = desc.flags;

                if (_enabled)
                    current |= _flags;
                else
                    current &= ~_flags;
                    
                if (current != desc.flags)
                {
                    desc.flags = current;
                    return true;
                }
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    EnumValueFlags Property::GetEnumValueFlags(u64 _value) const
    {
        for (uint i = 0; i < GetEnumCount(); ++i)
        {
            if (_value == GetUnsignedEnumValue(i))
            {
                const EnumDesc & desc = enums[i];
                return desc.flags;
            }
        }

        return (EnumValueFlags)0x0;
    }

    //--------------------------------------------------------------------------------------
    Property::Property(const Property & _other) :
        name(_other.name),
        className(_other.className),
        displayName(_other.displayName),
        defaultFolder(_other.defaultFolder),
        interfaceType(_other.interfaceType),
        description(_other.description),
        enumTypeName(_other.enumTypeName),
        type(_other.type),
        offset(_other.offset),
        sizeOf(_other.sizeOf),
        flags(_other.flags),
        range(_other.range),
        enums(_other.enums)
    {
        
    }

    //--------------------------------------------------------------------------------------
    Property::~Property()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void Property::SetInterface(const char * _interface)
    {
        interfaceType = _interface;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::GetInterface() const
    {
        return interfaceType;
    }

    //--------------------------------------------------------------------------------------
    void Property::SetRange(float2 _range)
    {
        if ((float)_range.x != (float)_range.y)
            flags |= PropertyFlags::HasRange;

        range = _range;
    }

    //--------------------------------------------------------------------------------------
    void Property::SetDefaultFolder(const char * _path)
    {
        defaultFolder = _path;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::GetDefaultFolder() const
    {
        return defaultFolder;
    }

    //--------------------------------------------------------------------------------------
    void Property::SetFlags(PropertyFlags _flagsToSet, PropertyFlags _flagsToRemove)
    {
        flags = (PropertyFlags)((std::underlying_type<PropertyFlags>::type(flags) & ~std::underlying_type<PropertyFlags>::type(_flagsToRemove)) | (std::underlying_type<PropertyFlags>::type(_flagsToSet)));
    }

    //--------------------------------------------------------------------------------------
    void Property::SetOffset(uint_ptr _offset)
    {
        offset = _offset;
    }

    //--------------------------------------------------------------------------------------
    void Property::SetDescription(const char * _description)
    {
        description = _description;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::GetDescription() const
    {
        return description;
    }

    //--------------------------------------------------------------------------------------
    void Property::SetEnumTypeName(const char * _enumTypeName)
    {
        // Skip namespaces if any
        if (char * last = strrchr((char *)_enumTypeName, ':'))
            enumTypeName = last + 1;
        else
            enumTypeName = _enumTypeName;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::GetEnumTypeName() const 
    {
        return enumTypeName;
    }

    //--------------------------------------------------------------------------------------
    u32 Property::GetEnumCount() const
    {
        return (u32)enums.size();
    }

    //--------------------------------------------------------------------------------------
    void Property::SetEnumName(uint index, core::string _name)
    {
        enums[index].name = _name;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::GetEnumName(uint index) const
    {
        return enums[index].name.c_str();
    }

    //--------------------------------------------------------------------------------------
    u64 Property::GetUnsignedEnumValue(uint index) const
    {
        VG_ASSERT(PropertyType::EnumU8 == GetType() || PropertyType::EnumU16 == GetType() || PropertyType::EnumU32 == GetType() || PropertyType::EnumU64 == GetType() || PropertyType::EnumFlagsU8 == GetType() || PropertyType::EnumFlagsU16 == GetType() || PropertyType::EnumFlagsU32 == GetType() || PropertyType::EnumFlagsU64 == GetType());
        return enums[index].value.u;
    }

    //--------------------------------------------------------------------------------------
    i64 Property::GetSignedEnumValue(uint index) const
    {
        VG_ASSERT(PropertyType::EnumI8 == GetType() || PropertyType::EnumI16 == GetType() || PropertyType::EnumI32 == GetType() || PropertyType::EnumI64 == GetType() || PropertyType::EnumFlagsI8 == GetType() || PropertyType::EnumFlagsI16 == GetType() || PropertyType::EnumFlagsI32 == GetType() || PropertyType::EnumFlagsI64 == GetType());
        return enums[index].value.s;
    }

    //--------------------------------------------------------------------------------------
    float2 Property::GetRange() const
    {
        return range;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::GetName() const
    {
        return name;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::GetClassName() const
    {
        return className;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::GetDisplayName() const
    {
        if (displayName)
            return displayName;
        else
            return name;
    }

    //--------------------------------------------------------------------------------------
    PropertyType Property::GetType() const
    {
        return type;
    }

    //--------------------------------------------------------------------------------------
    PropertyFlags Property::GetFlags() const
    {
        return flags;
    }

    //--------------------------------------------------------------------------------------
    uint_ptr Property::GetOffset() const
    {
        return offset;
    }

    //--------------------------------------------------------------------------------------
    core::u32 Property::GetSizeOf() const
    {
        return sizeOf;
    }

    //--------------------------------------------------------------------------------------
    PropertyLayoutElement Property::GetLayoutElementType() const
    {
        checkPropertyType(PropertyType::LayoutElement);
        auto type = (PropertyLayoutElement)sizeOf;
        VG_ASSERT(isEnumValue(type));
        return type;
    }

    //--------------------------------------------------------------------------------------
    bool * Property::GetPropertyBool(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Bool == GetType() || (PropertyType::LayoutElement == GetType() && asBool(PropertyFlags::Optional & GetFlags())));
        return (bool*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::i8 * Property::GetPropertyInt8(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Int8 == GetType() || PropertyType::EnumI8 == GetType() || PropertyType::EnumFlagsI8 == GetType());
        return (i8 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::i16 * Property::GetPropertyInt16(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Int16 == GetType()  || PropertyType::EnumI16 == GetType() || PropertyType::EnumFlagsI16 == GetType());
        return (i16 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::i32 * Property::GetPropertyInt32(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Int32 == GetType()  || PropertyType::EnumI32 == GetType() || PropertyType::EnumFlagsI32 == GetType());
        return (i32 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::i64 * Property::GetPropertyInt64(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Int64 == GetType()  || PropertyType::EnumI64 == GetType() || PropertyType::EnumFlagsI64 == GetType());
        return (i64 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::u8 * Property::GetPropertyUint8(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Uint8 == GetType() || PropertyType::EnumU8 == GetType() || PropertyType::EnumFlagsU8 == GetType());
        return (u8*)(uint_ptr(_object) + offset + _index * sizeof(u8));
    }

    //--------------------------------------------------------------------------------------
    core::u16 * Property::GetPropertyUint16(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Uint16 == GetType() || PropertyType::EnumU16 == GetType() || PropertyType::EnumFlagsU16 == GetType());
        return (u16*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::u32 * Property::GetPropertyUint32(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Uint32 == GetType() || PropertyType::EnumU32 == GetType() || PropertyType::EnumFlagsU32 == GetType());
        return (u32*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::u64 * Property::GetPropertyUint64(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Uint64 == GetType() || PropertyType::EnumU64 == GetType() || PropertyType::EnumFlagsU64 == GetType());
        return (u64 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    BitMask * Property::GetPropertyBitMask(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::BitMask);
        return (BitMask *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    ObjectHandle * Property::GetPropertyObjectHandle(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::ObjectHandle);
        return (ObjectHandle *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    float * Property::GetPropertyFloat(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::Float);
        return (float*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::float2 * Property::GetPropertyFloat2(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::Float2);
        return (float2 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::float3 * Property::GetPropertyFloat3(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::Float3);
        return (float3 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::float4 * Property::GetPropertyFloat4(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::Float4);
        VG_ASSERT(0 == _index || asBool(PropertyFlags::EnumArray & flags));
        return (float4*)(uint_ptr(_object) + offset + _index * sizeof(float4));
    }

    //--------------------------------------------------------------------------------------
    i32 * Property::GetPropertyIntN(const IObject * _object, uint _componentCount, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Int2 == GetType() || PropertyType::Int3 == GetType() || PropertyType::Int4 == GetType());
        VG_ASSERT(0 == _index || asBool(PropertyFlags::EnumArray & flags));
        return (i32 *)(uint_ptr(_object) + offset + _index * sizeof(float) * _componentCount);
    }

    //--------------------------------------------------------------------------------------
    u32 * Property::GetPropertyUintN(const IObject * _object, uint _componentCount, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Uint2 == GetType() || PropertyType::Uint3 == GetType() || PropertyType::Uint4 == GetType());
        VG_ASSERT(0 == _index || asBool(PropertyFlags::EnumArray & flags));
        return (u32 *)(uint_ptr(_object) + offset + _index * sizeof(float) * _componentCount);
    }

    //--------------------------------------------------------------------------------------
    float * Property::GetPropertyFloatN(const IObject * _object, uint _componentCount, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(PropertyType::Float2 == GetType() || PropertyType::Float3 == GetType() || PropertyType::Float4 == GetType());
        VG_ASSERT(0 == _index || asBool(PropertyFlags::EnumArray & flags));
        return (float *)(uint_ptr(_object) + offset + _index * sizeof(float) * _componentCount);
    }

    //--------------------------------------------------------------------------------------
    core::float4x4 * Property::GetPropertyFloat4x4(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::Float4x4);
        return (float4x4*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::string * Property::GetPropertyString(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::String);
        return (string*)(uint_ptr(_object) + offset + _index * this->GetSizeOf());
    }

    //--------------------------------------------------------------------------------------
    IResource * Property::GetPropertyResource(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::Resource);
        VG_ASSERT(0 == _index || asBool(PropertyFlags::EnumArray & flags));
        return (IResource*)(uint_ptr(_object) + offset + _index * this->GetSizeOf());
    }

    //--------------------------------------------------------------------------------------
    IResource ** Property::GetPropertyResourcePtr(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::ResourcePtr);
        VG_ASSERT(0 == _index || asBool(PropertyFlags::EnumArray & flags));
        return (IResource **)(uint_ptr(_object) + offset + _index * sizeof(IResource *));
    }

    //--------------------------------------------------------------------------------------
    void Property::checkPropertyType(PropertyType _type) const
    {
        VG_ASSERT(_type == GetType(), "The type of property \"%s\" is \"%s\" but its value of type \"%s\" is requested", GetName(), asString(GetType()).c_str(), asString(_type).c_str());
    }

    //--------------------------------------------------------------------------------------
    IObject * Property::GetPropertyObject(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::Object);
        VG_ASSERT(0 == _index || asBool(PropertyFlags::EnumArray & flags));
        return (IObject *)(uint_ptr(_object) + offset + _index * this->GetSizeOf());
    }

    //--------------------------------------------------------------------------------------
    IObject ** Property::GetPropertyObjectPtr(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::ObjectPtr);
        VG_ASSERT(0 == _index || asBool(PropertyFlags::EnumArray & flags));
        return (IObject**)(uint_ptr(_object) + offset + _index * sizeof(IObject*));
    }

    //--------------------------------------------------------------------------------------
    vector<IResource *> * Property::GetPropertyResourcePtrVector(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::ResourcePtrVector);
        return (vector<IResource *>*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    vector<IObject*> * Property::GetPropertyObjectPtrVector(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::ObjectPtrVector);
        return (vector<IObject*>*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    dictionary<IObject*> * Property::GetPropertyObjectPtrDictionary(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::ObjectPtrDictionary);
        return (dictionary<IObject*>*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    uint Property::GetPropertyObjectVectorCount(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::ObjectVector);
        return (uint)(((vector<u8>*)(uint_ptr(_object) + offset))->end() - ((vector<u8>*)(uint_ptr(_object) + offset))->begin()) / GetSizeOf();
    }

    //--------------------------------------------------------------------------------------
    u8 * Property::GetPropertyObjectVectorData(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::ObjectVector);
        return ((vector<u8>*)(uint_ptr(_object) + offset))->data();
    }

    //--------------------------------------------------------------------------------------
    IObject * Property::GetPropertyObjectVectorElement(const IObject * _object, uint _index) const
    {
        VG_ASSERT(_index<GetPropertyObjectVectorCount(_object)); 
        return (IObject *)(GetPropertyObjectVectorData(_object) + sizeOf * _index * GetSizeOf());
    }

    //--------------------------------------------------------------------------------------
    uint Property::GetPropertyResourceVectorCount(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::ResourceVector);
        return (uint)(((vector<u8>*)(uint_ptr(_object) + offset))->end() - ((vector<u8>*)(uint_ptr(_object) + offset))->begin()) / GetSizeOf();
    }

    //--------------------------------------------------------------------------------------
    u8 * Property::GetPropertyResourceVectorData(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(PropertyType::ResourceVector);
        return ((vector<u8>*)(uint_ptr(_object) + offset))->data();
    }

    //--------------------------------------------------------------------------------------
    IResource * Property::GetPropertyResourceVectorElement(const IObject * _object, uint _index) const
    {
        VG_ASSERT(_index < GetPropertyResourceVectorCount(_object));
        return (IResource *)(GetPropertyResourceVectorData(_object) + _index * GetSizeOf());
    }

    //--------------------------------------------------------------------------------------
    IProperty::Callback Property::GetPropertyCallback() const
    {
        checkPropertyType(PropertyType::Callback);
        return (IProperty::Callback)offset;
    }

    //--------------------------------------------------------------------------------------
    bool Property::IsResourceProperty() const
    {
        switch (type)
        {
            default:
            VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
            return false;

            case PropertyType::Undefined:
            case PropertyType::Bool:
            case PropertyType::Int8:
            case PropertyType::Int16:
            case PropertyType::Int32:
            case PropertyType::Int64:
            case PropertyType::Uint8:
            case PropertyType::Uint16:
            case PropertyType::Uint32:
            case PropertyType::Uint2:
            case PropertyType::Uint3:
            case PropertyType::Uint4:
            case PropertyType::Uint64:
            case PropertyType::Float:
            case PropertyType::Float2:
            case PropertyType::Float3:
            case PropertyType::Float4:
            case PropertyType::Float4x4:
            case PropertyType::String:
            case PropertyType::EnumU8:
            case PropertyType::EnumU16:
            case PropertyType::EnumU32:
            case PropertyType::EnumU64:
            case PropertyType::EnumFlagsU8:
            case PropertyType::EnumFlagsU16:
            case PropertyType::EnumFlagsU32:
            case PropertyType::EnumFlagsU64:
            case PropertyType::BitMask:
            case PropertyType::Callback:
            case PropertyType::LayoutElement:
            case PropertyType::Object:
            case PropertyType::ObjectPtr:
            case PropertyType::ObjectPtrVector:
            case PropertyType::ObjectPtrDictionary:
            case PropertyType::ObjectVector:
                return false;

            case PropertyType::Resource:
            case PropertyType::ResourcePtr:
            case PropertyType::ResourcePtrVector:
            case PropertyType::ResourceVector:
                return true;
            }
    }

    //--------------------------------------------------------------------------------------
    bool Property::IsObjectProperty() const
    {
        switch (type)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                return false;

            case PropertyType::Undefined:
            case PropertyType::Bool:
            case PropertyType::Int8:
            case PropertyType::Int16:
            case PropertyType::Int32:
            case PropertyType::Int64:
            case PropertyType::Uint8:
            case PropertyType::Uint16:
            case PropertyType::Uint32:
            case PropertyType::Uint2:
            case PropertyType::Uint3:
            case PropertyType::Uint4:
            case PropertyType::Uint64:
            case PropertyType::Float:
            case PropertyType::Float2:
            case PropertyType::Float3:
            case PropertyType::Float4:
            case PropertyType::Float4x4:
            case PropertyType::String:
            case PropertyType::EnumU8:
            case PropertyType::EnumU16:
            case PropertyType::EnumU32:
            case PropertyType::EnumU64:
            case PropertyType::EnumFlagsU8:
            case PropertyType::EnumFlagsU16:
            case PropertyType::EnumFlagsU32:
            case PropertyType::EnumFlagsU64:
            case PropertyType::BitMask:
            case PropertyType::Callback:
            case PropertyType::LayoutElement:
                return false;

            case PropertyType::Object:
            case PropertyType::ObjectPtr:
            case PropertyType::ObjectPtrVector:
            case PropertyType::ObjectPtrDictionary:
            case PropertyType::ObjectVector:
            case PropertyType::Resource:
            case PropertyType::ResourcePtr:
            case PropertyType::ResourcePtrVector:
            case PropertyType::ResourceVector:
                return true;
        }
    }
}