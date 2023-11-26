#pragma once

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Interface to PropertyDescriptor
    //--------------------------------------------------------------------------------------
    
    class IObject;
    class IResource;

    class IProperty
    {
    public:

        using Callback = bool (__cdecl*)(class IObject*);

        enum class Type : u32
        {
            Undefined = 0,
            Bool,
            Int8,
            Int16,
            Int32,
            Int64,
            Uint8,
            Uint16,
            Uint32,
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
            EnumFlagsU8,
            EnumFlagsU16,
            EnumFlagsU32,
            EnumFlagsU64,
            Resource,
            ResourceRef,
            Object,                 // Embedded IObject
            ObjectRef,              // Pointer to IObject
            ObjectRefVector,        // Vector of pointers to IObject
            ObjectRefDictionary,    // Dictionary of pointers to IObject

            // sizeof(element) is unknown (cannot be serialized for now as some array trickery is necessary to resize vector of unknown type)
            ObjectVector,
            ResourceVector,

            // No data, only (IObject*) callback
            Callback,
        };

        enum class Flags : u64
        {
            None            = 0x0000000000000000,
            ReadOnly        = 0x0000000000000001,   // Cannot edit from GUI
            Color           = 0x0000000000000002,   // Type represents a color (e.g. float4 or u32)
            IsFolder        = 0x0000000000000004,   // String is a folder
            HasRange        = 0x0000000000000008,   // Property has [min..max] range
            SameLine        = 0x0000000000000010,   // Do no end line after this property
			Radio		    = 0x0000000000000020,   // Part of a radio button group
            Hidden          = 0x0000000000000040,   // Hide property
            Bitfield        = 0x0000000000000080,   // Value displayed as hex, enum as flags
            Resource        = 0x0000000000000100,
            NotSaved        = 0x0000000000000200,
            EnumArray       = 0x0000000000000400,   // Property is fixed-size C array with element count the size of the enum
            Flatten         = 0x0000000000000800    // Do not open TreeNode to display object of this type
        };

        virtual void                        setRange                        (float2 _range) = 0;
        virtual void                        setFlags                        (Flags _flagsToSet, Flags _flagsToRemove = Flags::None) = 0;

        virtual const char *                getName                         () const = 0;
        virtual const char *                getClassName                    () const = 0;
        virtual const char *                getDisplayName                  () const = 0;
        virtual Type                        getType                         () const = 0;
        virtual Flags                       getFlags                        () const = 0;
        virtual uint_ptr                    getOffset                       () const = 0;
		virtual u32					        getSizeOf				        () const = 0;
        virtual float2                      getRange                        () const = 0;
        virtual u32                         getEnumCount                    () const = 0;
        virtual const char *                getEnumName                     (uint index) const = 0;
        virtual u64                         getEnumValue                    (uint index) const = 0;

        virtual bool *                      GetPropertyBool                 (const IObject * _object) const = 0;
        virtual u8 *                        GetPropertyUint8                (const IObject * _object) const = 0;
        virtual u16 *                       GetPropertyUint16               (const IObject * _object) const = 0;
        virtual u32 *                       GetPropertyUint32               (const IObject * _object) const = 0;
        virtual u64 *                       GetPropertyUint64               (const IObject * _object) const = 0;
        virtual float *                     GetPropertyFloat                (const IObject * _object) const = 0;
        virtual float2 *                    GetPropertyFloat2               (const IObject * _object) const = 0;
        virtual float3 *                    GetPropertyFloat3               (const IObject * _object) const = 0;
        virtual float4 *                    GetPropertyFloat4               (const IObject * _object, uint _index = 0) const = 0;
        virtual float4x4 *                  GetPropertyFloat4x4             (const IObject * _object) const = 0;
        virtual string *                    GetPropertyString               (const IObject * _object) const = 0;
        virtual IResource *                 GetPropertyResource             (const IObject * _object, uint _index = 0) const = 0;
        virtual IResource *                 GetPropertyResourceRef          (const IObject * _object, uint _index = 0) const = 0;
        virtual IObject *                   GetPropertyObject               (const IObject * _object, uint _index = 0) const = 0;
        virtual IObject *                   GetPropertyObjectRef            (const IObject * _object, uint _index = 0) const = 0;
        virtual vector<IObject*> *          GetPropertyObjectRefVector      (const IObject * _object) const = 0;
        virtual dictionary<IObject*> *      GetPropertyObjectRefDictionary  (const IObject * _object) const = 0;
        
        virtual uint                        GetPropertyObjectVectorCount    (const IObject * _object) const = 0;
        virtual u8 *                        GetPropertyObjectVectorData     (const IObject * _object) const = 0;
        virtual IObject *                   GetPropertyObjectVectorElement  (const IObject * _object, uint _index) const = 0;

        virtual uint                        GetPropertyResourceVectorCount  (const IObject * _object) const = 0;
        virtual u8 *                        GetPropertyResourceVectorData   (const IObject * _object) const = 0;
        virtual IResource *                 GetPropertyResourceVectorElement(const IObject * _object, uint _index) const = 0;

        virtual IProperty::Callback         GetPropertyCallback             () const = 0;

        template <typename E> inline E *    GetPropertyEnum                 (const IObject * _object) const { return (E *)GetPropertyUnderlyingType<std::underlying_type_t<E>>(_object); }


    private:
        template <typename T> inline T *    GetPropertyUnderlyingType       (const IObject * _object) const;
        template <> inline u8 *             GetPropertyUnderlyingType<u8>   (const IObject * _object) const { return GetPropertyUint8(_object); }
        template <> inline u16 *            GetPropertyUnderlyingType<u16>  (const IObject * _object) const { return GetPropertyUint16(_object); }
        template <> inline u32 *            GetPropertyUnderlyingType<u32>  (const IObject * _object) const { return GetPropertyUint32(_object); }
    }; 
}

//--------------------------------------------------------------------------------------
// Register object properties macros
// 'Ex' versions of the functions take flags as parameters
//--------------------------------------------------------------------------------------

// Register property for a simple type (e.g. int, float, float4 ...)
#define registerPropertyEx(className, propertyName, displayName, flags)								        RegisterProperty(#className, #propertyName, (&((className*)(nullptr))->propertyName), displayName, flags)
#define registerProperty(className, propertyName, displayName)									            registerPropertyEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)

// Register property for an Object or an Object container type
#define registerPropertyObjectEx(className, propertyName, displayName, flags)                               RegisterProperty(#className, #propertyName, (core::IObject*)offsetof(className, propertyName), displayName, flags);
#define registerPropertyObject(className, propertyName, displayName)                                        registerPropertyObjectEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)

#define registerPropertyObjectVectorEx(className, propertyName, elementType, displayName, flags)            RegisterProperty(#className, #propertyName, sizeof(elementType), &((className*)nullptr)->propertyName, displayName, flags);
#define registerPropertyObjectVector(className, propertyName, elementType, displayName)                     registerPropertyObjectVectorEx(className, propertyName, elementType, displayName, vg::core::IProperty::Flags::None)

// Register property for an Object* or an Object* container  type
#define registerPropertyObjectPtrEx(className, propertyName, displayName, flags)                            RegisterProperty(#className, #propertyName, (core::IObject**)offsetof(className, propertyName), displayName, flags);
#define registerPropertyObjectPtr(className, propertyName, displayName)                                     registerPropertyObjectPtrEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)

#define registerPropertyObjectPtrVectorEx(className, propertyName, displayName, flags)                      RegisterProperty(#className, #propertyName, (core::vector<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
#define registerPropertyObjectPtrVector(className, propertyName, displayName)                               registerPropertyObjectPtrVectorEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)

#define registerPropertyObjectPtrDictionaryEx(className, propertyName, displayName, flags)                  RegisterProperty(#className, #propertyName, (core::dictionary<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
#define registerPropertyObjectPtrDictionary(className, propertyName, displayName)                           registerPropertyObjectPtrDictionaryEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)

// Register property for an Resource* or an Resource* container  type
#define registerPropertyResourceEx(className, propertyName, displayName, flags)                             RegisterPropertyResource(#className, #propertyName, (core::IResource*) offsetof(className, propertyName), displayName, flags | vg::core::IProperty::Flags::Resource);
#define registerPropertyResource(className, propertyName, displayName)                                      registerPropertyResourceEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)    

#define registerPropertyResourcePtrEx(className, propertyName, displayName, flags)                          RegisterPropertyResourcePtr(#className, #propertyName, (core::IResource**)offsetof(className, propertyName), displayName, flags | vg::core::IProperty::Flags::Resource);
#define registerPropertyResourcePtr(className, propertyName, displayName)                                   registerPropertyResourcePtrEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)   

// Register property for a callback 
#define registerPropertyCallbackEx(className, funcName, displayName, flags)									RegisterProperty(#className, #funcName, funcName, displayName, flags)
#define registerPropertyCallback(className, funcName, displayName)	                                        registerPropertyCallbackEx(className, funcName, displayName, vg::core::IProperty::Flags::None)

// Register property for an enum
#define registerPropertyEnumEx(className, enumClassName, propertyName, displayName, flags)                  RegisterEnum(#className, #propertyName, (std::underlying_type_t<enumClassName>*)(&((className*)(nullptr))->propertyName), displayName, vg::core::EnumHelper<enumClassName>::getStaticCount(), vg::core::EnumHelper<enumClassName>::getStaticNames().c_str(), vg::core::EnumHelper<enumClassName>::getStaticValues().data(), flags);
#define registerPropertyEnum(className, enumClassName, propertyName, displayName)                           registerPropertyEnumEx(className, enumClassName, propertyName, displayName, vg::core::IProperty::Flags::None)

#define registerPropertyEnumBitfieldEx(className, enumClassName, propertyName, displayName, flags)          registerPropertyEnumEx(className, enumClassName, propertyName, displayName, vg::core::IProperty::Flags::Bitfield | flags)
#define registerPropertyEnumBitfield(className, enumClassName, propertyName, displayName)                   registerPropertyEnumEx(className, enumClassName, propertyName, displayName, vg::core::IProperty::Flags::Bitfield)

// Register property for fixed-size array using enum as index
#define registerPropertyEnumArray(className, elementType, enumClassName, propertyName, displayName, flags)  RegisterEnumArray(#className, #propertyName, (elementType*)(&((className*)(nullptr))->propertyName[0]), displayName, vg::core::EnumHelper<enumClassName>::getStaticCount(), sizeof(elementType), vg::core::EnumHelper<enumClassName>::getStaticNames().c_str(), (void*)vg::core::EnumHelper<enumClassName>::getStaticValues().data(), flags, vg::core::EnumHelper<enumClassName>::getSizeOfUnderlyingType());

//--------------------------------------------------------------------------------------
// Modify existing class properties macros
//--------------------------------------------------------------------------------------
#define setPropertyRange(className, propertyName, range)												    GetPropertyByName(#propertyName)->setRange(range);

//--------------------------------------------------------------------------------------
// Misc
//--------------------------------------------------------------------------------------
#define registerResizeVectorFunc(className, resizeVectorFunc)                                               RegisterResizeVectorFunc(#className, resizeVectorFunc);