#pragma once

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Interface to PropertyDescriptor
    //--------------------------------------------------------------------------------------
    
    class IObject;
    class IResource;
    class BitMask;
    class ObjectHandle;
    struct FloatCurveData;

    enum class EnumValueFlags : core::u32
    {
        Disabled    = 0x00000001,
        Hidden      = 0x00000002
    };

    vg_enum_class(vg::core, PropertyType, u32,
        Undefined = 0,
        Bool,
        Int8,
        Int16,
        Int32,
        Int64,
        Uint8,
        Uint16,
        Uint32,
        Uint2,
        Uint3,
        Uint4,
        Int2,
        Int3,
        Int4,
        Uint64,
        Float,
        Float2,
        Float3,
        Float4,
        Float4x4,
        String,
        EnumU8,
        EnumU16,
        EnumU32,
        EnumU64,
        EnumI8,
        EnumI16,
        EnumI32,
        EnumI64,
        EnumFlagsU8,
        EnumFlagsU16,
        EnumFlagsU32,
        EnumFlagsU64,
        BitMask,
        FloatCurveData,
        Resource,
        ResourcePtr,
        ResourcePtrVector,
        Object,                 // Embedded IObject
        ObjectHandle,           // For objects referencing other objects dynamically using UID
        ObjectPtr,              // Pointer to IObject
        ObjectPtrVector,        // Vector of pointers to IObject
        ObjectPtrDictionary,    // Dictionary of pointers to IObject,

        ObjectVector,           // sizeof(element) is unknown ('registerResizeVectorFunc' must be registered for serialization)
        ResourceVector,         // sizeof(element) is unknown ('registerResizeVectorFunc' must be registered for serialization)

        Callback,               // No data, only (IObject*) callback

        LayoutElement           // Cosmetic-only properties (doesn't change serialized data)
    );

    vg_enum_class(vg::core, PropertyFlags, u64,
        None                = 0x0000000000000000,
        ReadOnly            = 0x0000000000000001,   // A property that is visible, but edition is disabled
        Color               = 0x0000000000000002,   // Type represents a color (e.g., float4 or u32)
        IsFolder            = 0x0000000000000004,   // String is a folder
        IsFile              = 0x0000000000000008,   // String is a folder
        HasRange            = 0x0000000000000010,   // Property has [min..max] range
        SingleLine          = 0x0000000000000020,   // This property and the following ones will be displayed on the same line to save space
        Radio               = 0x0000000000000040,   // Part of a radio button group
        Debug               = 0x0000000000000080,   // A debug property that is hidden by default
        Bitfield            = 0x0000000000000100,   // Value displayed as hex, enum as flags
        Resource            = 0x0000000000000200,   // Property is a Resource
        Transient           = 0x0000000000000400,   // Property is not saved nor loaded
        EnumArray           = 0x0000000000000800,   // Property is fixed-size C array with element count the size of the enum
        Flatten             = 0x0000000000001000,   // Do not open TreeNode to display object of this type
        Optional            = 0x0000000000002000,   // Previous property must be a bool, and if 'false' then this value won't be editable
        HDR                 = 0x0000000000004000,   // HDR value for color
        Hidden              = 0x0000000000008000,   // A property that is not visible
        Hexadecimal         = 0x0000000000010000,   // Display value using hexadecimal
        EulerAngle          = 0x0000000000020000,   // Edited value is Euler angle
        AlphabeticalOrder   = 0x0000000000040000    // Sort multiple values in alphabetical order (e.g., enums)
    );

    vg_enum_class(vg::core, PropertyLayoutElement, u8,
        Separator = 0,
        SameLineBegin,
        SameLineEnd,
        GroupBegin,
        GroupEnd
    );

    class IObject;

    class IProperty
    {
    public:

        using ActionCallback             = bool (__cdecl*)(IObject*);
        using PropertyRangeCallback      = core::float2(__cdecl *)(const IObject *, const IProperty *, core::uint _index);
        using IsPropertyHiddenCallback   = bool(__cdecl *)(const IObject *, const IProperty *, core::uint _index);
        using IsPropertyReadOnlyCallback = bool(__cdecl *)(const IObject *, const IProperty *, core::uint _index);

        virtual void                            SetInterface                    (const char * _interface) = 0;
        virtual void                            SetRange                        (float2 _range) = 0;
        virtual void                            SetPropertyRangeCallback        (PropertyRangeCallback _func) = 0;
        virtual void                            SetPropertyHiddenCallback       (IsPropertyHiddenCallback _func) = 0;
        virtual void                            SetPropertyReadOnlyCallback     (IsPropertyReadOnlyCallback _func) = 0;
        virtual void                            SetDefaultFolder                (const char * _path) = 0;
        virtual void                            SetFlags                        (PropertyFlags _flagsToSet, PropertyFlags _flagsToRemove = PropertyFlags::None) = 0;
        virtual void                            SetOffset                       (uint_ptr _offset) = 0;
        virtual void                            SetDescription                  (const char * _description) = 0;
        virtual void                            SetEnumTypeName                 (const char * _enumTypeName) = 0;
        virtual bool                            SetEnumValueFlags               (u64 _value, EnumValueFlags _flags, bool _enabled) = 0;
        virtual EnumValueFlags                  GetUnsignedEnumValueFlags       (u64 _value) const = 0;
        virtual EnumValueFlags                  GetSignedEnumValueFlags         (i64 _value) const = 0;
        virtual const char *                    GetInterface                    () const = 0;
        virtual const char *                    GetName                         () const = 0;
        virtual const char *                    GetClassName                    () const = 0;
        virtual const char *                    GetDisplayName                  () const = 0;
        virtual const char *                    GetDefaultFolder                () const = 0;
        virtual const char *                    GetDescription                  () const = 0;
        virtual PropertyType                    GetType                         () const = 0;
        virtual PropertyFlags                   GetFlags                        () const = 0;
        virtual uint_ptr                        GetOffset                       () const = 0;
		virtual u32					            GetSizeOf				        () const = 0;
        virtual float2                          GetRange                        (const IObject * _object, core::uint _index = 0) const = 0;
        virtual bool                            IsHidden                        (const IObject * _object, core::uint _index = 0) const = 0;
        virtual bool                            IsReadOnly                      (const IObject * _object, core::uint _index = 0) const = 0;
        virtual const char *                    GetEnumTypeName                 () const = 0;
        virtual u32                             GetEnumCount                    () const = 0;
        virtual void                            SetEnumName                     (uint index, core::string _name) = 0;
        virtual const core::string &            GetEnumName                     (uint index) const = 0;
        virtual u64                             GetUnsignedEnumValue            (uint index) const = 0;
        virtual i64                             GetSignedEnumValue              (uint index) const = 0;

        virtual bool *                          GetPropertyBool                 (const IObject * _object, uint _index = 0) const = 0;

        virtual i8 *                            GetPropertyInt8                 (const IObject * _object) const = 0;
        virtual i16 *                           GetPropertyInt16                (const IObject * _object) const = 0;
        virtual i32 *                           GetPropertyInt32                (const IObject * _object) const = 0;
        virtual i64 *                           GetPropertyInt64                (const IObject * _object) const = 0;
        
        virtual u8 *                            GetPropertyUint8                (const IObject * _object, uint _index = 0) const = 0;
        virtual u16 *                           GetPropertyUint16               (const IObject * _object) const = 0;
        virtual u32 *                           GetPropertyUint32               (const IObject * _object) const = 0;
        virtual u64 *                           GetPropertyUint64               (const IObject * _object) const = 0;
        
        virtual i32 *                           GetPropertyIntN                 (const IObject * _object, uint _componentCount, uint _index = 0) const = 0;
        virtual u32 *                           GetPropertyUintN                (const IObject * _object, uint _componentCount, uint _index = 0) const = 0;
        
        virtual FloatCurveData *                GetPropertyFloatCurveData       (const IObject * _object) const = 0;
        
        virtual BitMask *                       GetPropertyBitMask              (const IObject * _object) const = 0;
        virtual ObjectHandle *                  GetPropertyObjectHandle         (const IObject * _object) const = 0;

        virtual float *                         GetPropertyFloat                (const IObject * _object) const = 0;
        virtual float2 *                        GetPropertyFloat2               (const IObject * _object) const = 0;
        virtual float3 *                        GetPropertyFloat3               (const IObject * _object) const = 0;
        virtual float4 *                        GetPropertyFloat4               (const IObject * _object, uint _index = 0) const = 0;
        virtual float *                         GetPropertyFloatN               (const IObject * _object, uint _componentCount, uint _index = 0) const = 0;
        virtual float4x4 *                      GetPropertyFloat4x4             (const IObject * _object) const = 0;
        virtual string *                        GetPropertyString               (const IObject * _object, uint _index = 0) const = 0;
        virtual IResource *                     GetPropertyResource             (const IObject * _object, uint _index = 0) const = 0;
        virtual vector<IResource *> *           GetPropertyResourcePtrVector    (const IObject * _object) const = 0;
        virtual IResource **                    GetPropertyResourcePtr          (const IObject * _object, uint _index = 0) const = 0;
        virtual IObject *                       GetPropertyObject               (const IObject * _object, uint _index = 0) const = 0;
        virtual IObject **                      GetPropertyObjectPtr            (const IObject * _object, uint _index = 0) const = 0;
        virtual vector<IObject*> *              GetPropertyObjectPtrVector      (const IObject * _object) const = 0;
        virtual dictionary<IObject*> *          GetPropertyObjectPtrDictionary  (const IObject * _object) const = 0;
        
        virtual uint                            GetPropertyObjectVectorCount    (const IObject * _object) const = 0;
        virtual u8 *                            GetPropertyObjectVectorData     (const IObject * _object) const = 0;
        virtual IObject *                       GetPropertyObjectVectorElement  (const IObject * _object, uint _index) const = 0;

        virtual uint                            GetPropertyResourceVectorCount  (const IObject * _object) const = 0;
        virtual u8 *                            GetPropertyResourceVectorData   (const IObject * _object) const = 0;
        virtual IResource *                     GetPropertyResourceVectorElement(const IObject * _object, uint _index) const = 0;

        virtual IProperty::ActionCallback       GetPropertyActionCallback       () const = 0;

        virtual PropertyLayoutElement           GetLayoutElementType            () const = 0;

        virtual bool                            IsResourceProperty              () const = 0;
        virtual bool                            IsObjectProperty                () const = 0;

        template <typename E> inline E *        GetPropertyEnum                 (const IObject * _object) const { return (E *)GetPropertyUnderlyingType<std::underlying_type_t<E>>(_object); }

    private:
        template <typename T> inline T *        GetPropertyUnderlyingType       (const IObject * _object) const;

        template <> inline i8 *                 GetPropertyUnderlyingType<i8>   (const IObject * _object) const { return GetPropertyInt8(_object); }
        template <> inline i16 *                GetPropertyUnderlyingType<i16>  (const IObject * _object) const { return GetPropertyInt16(_object); }
        template <> inline i32 *                GetPropertyUnderlyingType<i32>  (const IObject * _object) const { return GetPropertyInt32(_object); }
        template <> inline i64 *                GetPropertyUnderlyingType<i64>  (const IObject * _object) const { return GetPropertyInt64(_object); }

        template <> inline u8 *                 GetPropertyUnderlyingType<u8>   (const IObject * _object) const { return GetPropertyUint8(_object); }
        template <> inline u16 *                GetPropertyUnderlyingType<u16>  (const IObject * _object) const { return GetPropertyUint16(_object); }
        template <> inline u32 *                GetPropertyUnderlyingType<u32>  (const IObject * _object) const { return GetPropertyUint32(_object); }
        template <> inline u64 *                GetPropertyUnderlyingType<u64>  (const IObject * _object) const { return GetPropertyUint64(_object); }
    }; 
}

//--------------------------------------------------------------------------------------
// Register object properties macros
// 'Ex' versions of the functions take flags as parameters
//--------------------------------------------------------------------------------------

// Register property for a simple type (e.g., int, float, float4 ...)
#define registerPropertyEx(className, propertyName, displayName, flags)								        _desc.RegisterProperty(#className, #propertyName, (&((className*)(nullptr))->propertyName), displayName, flags)
#define registerProperty(className, propertyName, displayName)									            registerPropertyEx(className, propertyName, displayName, vg::core::PropertyFlags::None)

// Register property for an Object or an Object container type
#define registerPropertyObjectEx(className, propertyName, displayName, flags)                               _desc.RegisterProperty(#className, #propertyName, (core::IObject*)VG_OFFSETOF(className, propertyName), displayName, flags);
#define registerPropertyObject(className, propertyName, displayName)                                        registerPropertyObjectEx(className, propertyName, displayName, vg::core::PropertyFlags::None)

#define registerPropertyObjectVectorEx(className, propertyName, elementType, displayName, flags)            _desc.RegisterProperty(#className, #propertyName, sizeof(elementType), &((className*)nullptr)->propertyName, displayName, flags);
#define registerPropertyObjectVector(className, propertyName, elementType, displayName)                     registerPropertyObjectVectorEx(className, propertyName, elementType, displayName, vg::core::PropertyFlags::None)

// Register property for an Object* or an Object* container  type
#define registerPropertyObjectPtrEx(className, propertyName, displayName, flags)                            _desc.RegisterProperty(#className, #propertyName, (core::IObject**)VG_OFFSETOF(className, propertyName), displayName, flags);
#define registerPropertyObjectPtr(className, propertyName, displayName)                                     registerPropertyObjectPtrEx(className, propertyName, displayName, vg::core::PropertyFlags::None)

#define registerPropertyObjectPtrVectorEx(className, propertyName, displayName, flags)                      _desc.RegisterPropertyT(#className, #propertyName, &((className*)nullptr)->propertyName, displayName, flags);
#define registerPropertyObjectPtrVector(className, propertyName, displayName)                               registerPropertyObjectPtrVectorEx(className, propertyName, displayName, vg::core::PropertyFlags::None)

#define registerPropertyObjectPtrDictionaryEx(className, propertyName, displayName, flags)                  _desc.RegisterProperty(#className, #propertyName, (core::dictionary<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
#define registerPropertyObjectPtrDictionary(className, propertyName, displayName)                           registerPropertyObjectPtrDictionaryEx(className, propertyName, displayName, vg::core::PropertyFlags::None)

// Register property for an Resource* or an Resource* container  type
#define registerPropertyResourceEx(className, propertyName, displayName, flags)                             _desc.RegisterPropertyResource(#className, #propertyName, (core::IResource*)VG_OFFSETOF(className, propertyName), displayName, flags | vg::core::PropertyFlags::Resource);
#define registerPropertyResource(className, propertyName, displayName)                                      registerPropertyResourceEx(className, propertyName, displayName, vg::core::PropertyFlags::None)    

#define registerPropertyResourceVectorEx(className, propertyName, elementType, displayName, flags)          registerPropertyObjectVectorEx(className, propertyName, elementType, displayName, flags | vg::core::PropertyFlags::Resource)
#define registerPropertyResourceVector(className, propertyName, elementType, displayName)                   registerPropertyResourceVectorEx(className, propertyName, elementType, displayName, vg::core::PropertyFlags::None)    

#define registerPropertyResourcePtrEx(className, propertyName, displayName, flags)                          _desc.RegisterPropertyResourcePtr(#className, #propertyName, (core::IResource**)VG_OFFSETOF(className, propertyName), displayName, flags | vg::core::PropertyFlags::Resource);
#define registerPropertyResourcePtr(className, propertyName, displayName)                                   registerPropertyResourcePtrEx(className, propertyName, displayName, vg::core::PropertyFlags::None)   

#define registerPropertyResourcePtrVectorEx(className, propertyName, displayName, flags)                    registerPropertyObjectPtrVectorEx(className, propertyName, displayName, flags | vg::core::PropertyFlags::Resource) 
#define registerPropertyResourcePtrVector(className, propertyName, displayName)                             registerPropertyResourcePtrVectorEx(className, propertyName, displayName, vg::core::PropertyFlags::None)   

// Register property for a callback 
#define registerPropertyCallbackEx(className, funcName, displayName, flags)									_desc.RegisterProperty(#className, #funcName, funcName, displayName, flags)
#define registerPropertyCallback(className, funcName, displayName)	                                        registerPropertyCallbackEx(className, funcName, displayName, vg::core::PropertyFlags::None)

// Register property for an enum
#define registerPropertyEnumEx(className, enumClassName, propertyName, displayName, flags)                  _desc.RegisterEnum(#className, #propertyName, #enumClassName, (std::underlying_type_t<enumClassName>*)(&((className*)(nullptr))->propertyName), displayName, vg::core::EnumHelper<enumClassName>::getStaticCount(), vg::core::EnumHelper<enumClassName>::getStaticNames().c_str(), vg::core::EnumHelper<enumClassName>::getStaticValues().data(), flags);
#define registerPropertyEnum(className, enumClassName, propertyName, displayName)                           registerPropertyEnumEx(className, enumClassName, propertyName, displayName, vg::core::PropertyFlags::None)

#define registerPropertyEnumBitfieldEx(className, enumClassName, propertyName, displayName, flags)          registerPropertyEnumEx(className, enumClassName, propertyName, displayName, vg::core::PropertyFlags::Bitfield | flags)
#define registerPropertyEnumBitfield(className, enumClassName, propertyName, displayName)                   registerPropertyEnumEx(className, enumClassName, propertyName, displayName, vg::core::PropertyFlags::Bitfield)

// Register property for fixed-size array using enum as index
#define registerPropertyEnumArrayEx(className, elementType, enumClassName, propertyName, displayName, flags) _desc.RegisterEnumArray(#className, #propertyName, (elementType*)(&((className*)(nullptr))->propertyName[0]), displayName, vg::core::EnumHelper<enumClassName>::getStaticCount(), sizeof(elementType), vg::core::EnumHelper<enumClassName>::getStaticNames().c_str(), (void*)vg::core::EnumHelper<enumClassName>::getStaticValues().data(), flags, vg::core::EnumHelper<enumClassName>::getSizeOfUnderlyingType());
#define registerPropertyEnumArray(className, elementType, enumClassName, propertyName, displayName)         registerPropertyEnumArrayEx(className, elementType, enumClassName, propertyName, displayName, vg::core::PropertyFlags::None);

// Helper for "optional" property (bool + property)
#define registerOptionalPropertyEx(className, boolpropertyname, propertyName, displayName, flags)  	        registerPropertyEx(className, boolpropertyname, displayName, PropertyFlags::Hidden); \
                                                                                                            registerPropertyEx(className, propertyName, displayName, PropertyFlags::Optional | flags);

#define registerOptionalProperty(className, boolpropertyname, propertyName, displayName)	                registerOptionalPropertyEx(className, boolpropertyname, propertyName, displayName, vg::core::PropertyFlags::None)

#define registerOptionalPropertyEnum(className, boolpropertyname, enumClassName, propertyName, displayName)             registerPropertyEx(className, boolpropertyname, displayName, PropertyFlags::Hidden); \
                                                                                                                        registerPropertyEnumEx(className, enumClassName, propertyName, displayName, vg::core::PropertyFlags::Optional) 


#define registerOptionalPropertyEnumBitfield(className, boolpropertyname, enumClassName, propertyName, displayName)     registerPropertyEx(className, boolpropertyname, displayName, PropertyFlags::Hidden); \
                                                                                                                        registerPropertyEnumEx(className, enumClassName, propertyName, displayName, vg::core::PropertyFlags::Bitfield | vg::core::PropertyFlags::Optional) 

#define registerOptionalPropertyResource(className, boolpropertyname, propertyName, displayName)            registerPropertyEx(className, boolpropertyname, displayName, PropertyFlags::Hidden); \
                                                                                                            registerPropertyResourceEx(className, propertyName, displayName, vg::core::PropertyFlags::Optional)    

#define registerOptionalPropertyResourcePtr(className, boolpropertyname, propertyName, displayName)         registerPropertyEx(className, boolpropertyname, displayName, PropertyFlags::Hidden); \
                                                                                                            registerPropertyResourcePtrEx(className, propertyName, displayName, vg::core::PropertyFlags::Optional)  

//--------------------------------------------------------------------------------------
// Modify existing class properties macros
//--------------------------------------------------------------------------------------
#define setPropertyFlag(className, propertyName, flags, value)												{ if (auto * prop = _desc.GetPropertyByName(#propertyName)) { prop->SetFlags(value ? flags : (vg::core::PropertyFlags)0, value ? (vg::core::PropertyFlags)0 : flags); } else { VG_WARNING("[Factory] Could not set \"Flags\" for property \"%s\" in class \"%s\"", #propertyName, #className); } }
#define setPropertyRange(className, propertyName, range)												    { if (auto * prop = _desc.GetPropertyByName(#propertyName)) { prop->SetRange(range);                                                                                  } else { VG_WARNING("[Factory] Could not set \"Range\" for property \"%s\" in class \"%s\"", #propertyName, #className); } }
#define setPropertyRangeCallback(className, propertyName, func)                                             { if (auto * prop = _desc.GetPropertyByName(#propertyName)) { prop->SetPropertyRangeCallback(func);                                                                   } else { VG_WARNING("[Factory] Could not set \"Range Callback\" for property \"%s\" in class \"%s\"", #propertyName, #className); } }
#define setPropertyHiddenCallback(className, propertyName, func)                                            { if (auto * prop = _desc.GetPropertyByName(#propertyName)) { prop->SetPropertyHiddenCallback(func);                                                                  } else { VG_WARNING("[Factory] Could not set \"Range Callback\" for property \"%s\" in class \"%s\"", #propertyName, #className); } }
#define setPropertyReadOnlyCallback(className, propertyName, func)                                          { if (auto * prop = _desc.GetPropertyByName(#propertyName)) { prop->SetPropertyReadOnlyCallback(func);                                                                } else { VG_WARNING("[Factory] Could not set \"Range Callback\" for property \"%s\" in class \"%s\"", #propertyName, #className); } }
#define setPropertyDefaultFolder(className, propertyName, defaultFolder)									{ if (auto * prop = _desc.GetPropertyByName(#propertyName)) { prop->SetDefaultFolder(defaultFolder);                                                                  } else { VG_WARNING("[Factory] Could not set \"Default Folder\" for property \"%s\" in class \"%s\"", #propertyName, #className); } }
#define setPropertyDescription(className, propertyName, description)                                        { if (auto * prop = _desc.GetPropertyByName(#propertyName)) { prop->SetDescription(description);                                                                      } else { VG_WARNING("[Factory] Could not set \"Description\" for property \"%s\" in class \"%s\"", #propertyName, #className); } }

//--------------------------------------------------------------------------------------
// Misc
//--------------------------------------------------------------------------------------
#define registerResizeVectorFunc(className, resizeVectorFunc)                                               _desc.RegisterResizeVectorFunc(#className, resizeVectorFunc);

#define registerPropertySeparator(className, label)                                                         _desc.RegisterPropertyLayout(#className, vg::core::PropertyLayoutElement::Separator, label, nullptr, vg::core::PropertyFlags::None);

#define registerPropertyGroupBegin(className, label)                                                        _desc.RegisterPropertyLayout(#className, vg::core::PropertyLayoutElement::GroupBegin, label, nullptr, vg::core::PropertyFlags::None);
#define registerPropertyGroupEnd(className)                                                                 _desc.RegisterPropertyLayout(#className, vg::core::PropertyLayoutElement::GroupEnd, "", nullptr, vg::core::PropertyFlags::None);

#define registerPropertyOptionalGroupBegin(className, propertyName, label)                                  _desc.RegisterPropertyLayout(#className, vg::core::PropertyLayoutElement::GroupBegin, label, (bool*)(&((className*)(nullptr))->propertyName), vg::core::PropertyFlags::Optional);
#define registerPropertyOptionalGroupEnd(className)                                                         _desc.RegisterPropertyLayout(#className, vg::core::PropertyLayoutElement::GroupEnd, "", nullptr, vg::core::PropertyFlags::Optional);