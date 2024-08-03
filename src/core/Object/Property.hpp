#include "Property.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Property::Property(const char * _class, const char * _name, Type _type, uint_ptr _offset, core::u32 _sizeOf, const char * _prettyName, Flags _flags, uint _enumCount, const char * _enumNames, const void * _enumValues, uint _enumSizeOf) :
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
                case Type::EnumU64:
                case Type::EnumFlagsU64:
                    initEnum<u64>(_enumCount, _enumNames, _enumValues);
                    break;

                // Types that support enumArray
                case Type::Resource:
                case Type::Uint8:
                case Type::Float4:
                    if (asBool(IProperty::Flags::EnumArray & _flags))
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
                        VG_ASSERT(false, "[Property] EnumArray not implemented for type \"%s\"", _type);
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
            enums[e].value = ((T*)_enumValues)[e];
        }
    }

    //--------------------------------------------------------------------------------------
    Property::Property(const Property & _other)
    {
        name = _other.name;
        className = _other.className;
        displayName = _other.displayName;
        defaultFolder = _other.defaultFolder;
        interfaceType = _other.interfaceType;
        type = _other.type;
        offset = _other.offset;
        sizeOf = _other.sizeOf;
        flags = _other.flags;
        range = _other.range;
        enums = _other.enums;
    }

    //--------------------------------------------------------------------------------------
    Property::~Property()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void Property::setInterface(const char * _interface)
    {
        interfaceType = _interface;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::getInterface() const
    {
        return interfaceType;
    }

    //--------------------------------------------------------------------------------------
    void Property::setRange(float2 _range)
    {
        if ((float)_range.x != (float)_range.y)
            flags |= IProperty::Flags::HasRange;

        range = _range;
    }

    //--------------------------------------------------------------------------------------
    void Property::setDefaultFolder(const char * _path)
    {
        defaultFolder = _path;
    }

    //--------------------------------------------------------------------------------------
    const char * Property::getDefaultFolder() const
    {
        return defaultFolder;
    }

    //--------------------------------------------------------------------------------------
    void Property::setFlags(Flags _flagsToSet, Flags _flagsToRemove)
    {
        flags = (Flags)((std::underlying_type<Flags>::type(flags) & ~std::underlying_type<Flags>::type(_flagsToRemove)) | (std::underlying_type<Flags>::type(_flagsToSet)));
    }

    //--------------------------------------------------------------------------------------
    void Property::setOffset(uint_ptr _offset)
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
    u64 Property::getEnumValue(uint index) const
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
    const char * Property::GetClassName() const
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
    IProperty::LayoutElementType Property::GetLayoutElementType() const
    {
        checkPropertyType(Type::LayoutElement);
        auto type = (IProperty::LayoutElementType)sizeOf;
        VG_ASSERT(isEnumValue(type));
        return type;
    }

    //--------------------------------------------------------------------------------------
    bool * Property::GetPropertyBool(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Bool == getType() || (Type::LayoutElement == getType() && asBool(IProperty::Flags::Optional & getFlags())));
        return (bool*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::i8 * Property::GetPropertyInt8(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Int8 == getType() /* || Type::EnumI8 == getType() || Type::EnumFlagsI8 == getType()*/);
        return (i8 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::i16 * Property::GetPropertyInt16(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Int16 == getType() /* || Type::EnumI16 == getType() || Type::EnumFlagsI16 == getType()*/);
        return (i16 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::i32 * Property::GetPropertyInt32(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Int32 == getType() /* || Type::EnumI32 == getType() || Type::EnumFlagsI32 == getType()*/);
        return (i32 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::i64 * Property::GetPropertyInt64(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Int64 == getType() /* || Type::EnumI64 == getType() || Type::EnumFlagsI64 == getType()*/);
        return (i64 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::u8 * Property::GetPropertyUint8(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Uint8 == getType() || Type::EnumU8 == getType() || Type::EnumFlagsU8 == getType());
        return (u8*)(uint_ptr(_object) + offset + _index * sizeof(u8));
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
    BitMask * Property::GetPropertyBitMask(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::BitMask);
        return (BitMask *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    ObjectHandle * Property::GetPropertyObjectHandle(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::ObjectHandle);
        return (ObjectHandle *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    float * Property::GetPropertyFloat(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::Float);
        return (float*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::float2 * Property::GetPropertyFloat2(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::Float2);
        return (float2 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::float3 * Property::GetPropertyFloat3(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::Float3);
        return (float3 *)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::float4 * Property::GetPropertyFloat4(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::Float4);
        VG_ASSERT(0 == _index || asBool(Flags::EnumArray & flags));
        return (float4*)(uint_ptr(_object) + offset + _index * sizeof(float4));
    }

    //--------------------------------------------------------------------------------------
    i32 * Property::GetPropertyIntN(const IObject * _object, uint _componentCount, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Int2 == getType() || Type::Int3 == getType() || Type::Int4 == getType());
        VG_ASSERT(0 == _index || asBool(Flags::EnumArray & flags));
        return (i32 *)(uint_ptr(_object) + offset + _index * sizeof(float) * _componentCount);
    }

    //--------------------------------------------------------------------------------------
    u32 * Property::GetPropertyUintN(const IObject * _object, uint _componentCount, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Uint2 == getType() || Type::Uint3 == getType() || Type::Uint4 == getType());
        VG_ASSERT(0 == _index || asBool(Flags::EnumArray & flags));
        return (u32 *)(uint_ptr(_object) + offset + _index * sizeof(float) * _componentCount);
    }

    //--------------------------------------------------------------------------------------
    float * Property::GetPropertyFloatN(const IObject * _object, uint _componentCount, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        VG_ASSERT(Type::Float2 == getType() || Type::Float3 == getType() || Type::Float4 == getType());
        VG_ASSERT(0 == _index || asBool(Flags::EnumArray & flags));
        return (float *)(uint_ptr(_object) + offset + _index * sizeof(float) * _componentCount);
    }

    //--------------------------------------------------------------------------------------
    core::float4x4 * Property::GetPropertyFloat4x4(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::Float4x4);
        return (float4x4*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    core::string * Property::GetPropertyString(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::String);
        return (string*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    IResource * Property::GetPropertyResource(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::Resource);
        VG_ASSERT(0 == _index || asBool(Flags::EnumArray & flags));
        return (IResource*)(uint_ptr(_object) + offset + _index * this->getSizeOf());
    }

    //--------------------------------------------------------------------------------------
    IResource ** Property::GetPropertyResourcePtr(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::ResourcePtr);
        VG_ASSERT(0 == _index || asBool(Flags::EnumArray & flags));
        return (IResource **)(uint_ptr(_object) + offset + _index * sizeof(IResource *));
    }

    //--------------------------------------------------------------------------------------
    void Property::checkPropertyType(Type _type) const
    {
        VG_ASSERT(_type == getType(), "The type of property \"%s\" is \"%s\" but its value of type \"%s\" is requested", getName(), asString(getType()).c_str(), asString(_type).c_str());
    }

    //--------------------------------------------------------------------------------------
    IObject * Property::GetPropertyObject(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::Object);
        VG_ASSERT(0 == _index || asBool(Flags::EnumArray & flags));
        return (IObject *)(uint_ptr(_object) + offset + _index * this->getSizeOf());
    }

    //--------------------------------------------------------------------------------------
    IObject ** Property::GetPropertyObjectPtr(const IObject * _object, uint _index) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::ObjectPtr);
        VG_ASSERT(0 == _index || asBool(Flags::EnumArray & flags));
        return (IObject**)(uint_ptr(_object) + offset + _index * sizeof(IObject*));
    }

    //--------------------------------------------------------------------------------------
    vector<IResource *> * Property::GetPropertyResourcePtrVector(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::ResourcePtrVector);
        return (vector<IResource *>*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    vector<IObject*> * Property::GetPropertyObjectPtrVector(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::ObjectPtrVector);
        return (vector<IObject*>*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    dictionary<IObject*> * Property::GetPropertyObjectPtrDictionary(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::ObjectPtrDictionary);
        return (dictionary<IObject*>*)(uint_ptr(_object) + offset);
    }

    //--------------------------------------------------------------------------------------
    uint Property::GetPropertyObjectVectorCount(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::ObjectVector);
        return (uint)(((vector<u8>*)(uint_ptr(_object) + offset))->end() - ((vector<u8>*)(uint_ptr(_object) + offset))->begin()) / getSizeOf();
    }

    //--------------------------------------------------------------------------------------
    u8 * Property::GetPropertyObjectVectorData(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::ObjectVector);
        return ((vector<u8>*)(uint_ptr(_object) + offset))->data();
    }

    //--------------------------------------------------------------------------------------
    IObject * Property::GetPropertyObjectVectorElement(const IObject * _object, uint _index) const
    {
        VG_ASSERT(_index<GetPropertyObjectVectorCount(_object)); 
        return (IObject *)(GetPropertyObjectVectorData(_object) + sizeOf * _index * getSizeOf());
    }

    //--------------------------------------------------------------------------------------
    uint Property::GetPropertyResourceVectorCount(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::ResourceVector);
        return (uint)(((vector<u8>*)(uint_ptr(_object) + offset))->end() - ((vector<u8>*)(uint_ptr(_object) + offset))->begin()) / getSizeOf();
    }

    //--------------------------------------------------------------------------------------
    u8 * Property::GetPropertyResourceVectorData(const IObject * _object) const
    {
        VG_ASSERT(nullptr != _object);
        checkPropertyType(Type::ResourceVector);
        return ((vector<u8>*)(uint_ptr(_object) + offset))->data();
    }

    //--------------------------------------------------------------------------------------
    IResource * Property::GetPropertyResourceVectorElement(const IObject * _object, uint _index) const
    {
        VG_ASSERT(_index < GetPropertyResourceVectorCount(_object));
        return (IResource *)(GetPropertyResourceVectorData(_object) + _index * getSizeOf());
    }

    //--------------------------------------------------------------------------------------
    IProperty::Callback Property::GetPropertyCallback() const
    {
        checkPropertyType(Type::Callback);
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

            case Type::Undefined:
            case Type::Bool:
            case Type::Int8:
            case Type::Int16:
            case Type::Int32:
            case Type::Int64:
            case Type::Uint8:
            case Type::Uint16:
            case Type::Uint32:
            case Type::Uint2:
            case Type::Uint3:
            case Type::Uint4:
            case Type::Uint64:
            case Type::Float:
            case Type::Float2:
            case Type::Float3:
            case Type::Float4:
            case Type::Float4x4:
            case Type::String:
            case Type::EnumU8:
            case Type::EnumU16:
            case Type::EnumU32:
            case Type::EnumU64:
            case Type::EnumFlagsU8:
            case Type::EnumFlagsU16:
            case Type::EnumFlagsU32:
            case Type::EnumFlagsU64:
            case Type::BitMask:
            case Type::Callback:
            case Type::LayoutElement:
            case Type::Object:
            case Type::ObjectPtr:
            case Type::ObjectPtrVector:
            case Type::ObjectPtrDictionary:
            case Type::ObjectVector:
                return false;

            case Type::Resource:
            case Type::ResourcePtr:
            case Type::ResourcePtrVector:
            case Type::ResourceVector:
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

            case Type::Undefined:
            case Type::Bool:
            case Type::Int8:
            case Type::Int16:
            case Type::Int32:
            case Type::Int64:
            case Type::Uint8:
            case Type::Uint16:
            case Type::Uint32:
            case Type::Uint2:
            case Type::Uint3:
            case Type::Uint4:
            case Type::Uint64:
            case Type::Float:
            case Type::Float2:
            case Type::Float3:
            case Type::Float4:
            case Type::Float4x4:
            case Type::String:
            case Type::EnumU8:
            case Type::EnumU16:
            case Type::EnumU32:
            case Type::EnumU64:
            case Type::EnumFlagsU8:
            case Type::EnumFlagsU16:
            case Type::EnumFlagsU32:
            case Type::EnumFlagsU64:
            case Type::BitMask:
            case Type::Callback:
            case Type::LayoutElement:
                return false;

            case Type::Object:
            case Type::ObjectPtr:
            case Type::ObjectPtrVector:
            case Type::ObjectPtrDictionary:
            case Type::ObjectVector:
            case Type::Resource:
            case Type::ResourcePtr:
            case Type::ResourcePtrVector:
            case Type::ResourceVector:
                return true;
        }
    }
}