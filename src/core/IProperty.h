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
            Uint8,
            Uint16,
            Uint32,
            Float,
            Float4,
            Float4x4,
            String,
            EnumU8,
            EnumU16,
            EnumU32,
            EnumFlagsU8,
            EnumFlagsU16,
            EnumFlagsU32,
            Resource,
            ObjectRef,
            ObjectRefVector,
            ObjectRefDictionary,

            // sizeof(element) is unknown
            ObjectVector,
            ResourceVector,

            // No data, only (IObject*) callback
            Callback,
        };

        enum class Flags : u64
        {
            None        = 0x0000000000000000,
            ReadOnly    = 0x0000000000000001,   // Cannot edit from GUI
            Color       = 0x0000000000000002,   // Type represents a color
            IsFolder    = 0x0000000000000004,   // String is a folder
            HasRange    = 0x0000000000000008,   // Property has [min..max] range
            SameLine    = 0x0000000000000010,   // Do no end line after this property
			Radio		= 0x0000000000000020,   // Part of a radio button group
            Hidden      = 0x0000000000000040,   // Hide property
            Bitfield    = 0x0000000000000080,   // Value displayed as hex, enum as flags
            Resource    = 0x0000000000000100,
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
        virtual u32                         getEnumValue                    (uint index) const = 0;

        virtual bool *                      GetPropertyBool                 (const IObject * _object) const = 0;
        virtual u8 *                        GetPropertyUint8                (const IObject * _object) const = 0;
        virtual u16 *                       GetPropertyUint16               (const IObject * _object) const = 0;
        virtual u32 *                       GetPropertyUint32               (const IObject * _object) const = 0;
        virtual float *                     GetPropertyFloat                (const IObject * _object) const = 0;
        virtual float4 *                    GetPropertyFloat4               (const IObject * _object) const = 0;
        virtual float4x4 *                  GetPropertyFloat4x4             (const IObject * _object) const = 0;
        virtual string *                    GetPropertyString               (const IObject * _object) const = 0;
        virtual IResource *                 GetPropertyResource             (const IObject * _object) const = 0;
        virtual IObject *                   GetPropertyObjectRef            (const IObject * _object) const = 0;
        virtual vector<IObject*> *          GetPropertyObjectRefVector      (const IObject * _object) const = 0;
        virtual dictionary<IObject*> *      GetPropertyObjectRefDictionary  (const IObject * _object) const = 0;

        virtual uint                        GetPropertyObjectVectorCount    (const IObject * _object) const = 0;
        virtual u8 *                        GetPropertyObjectVectorData     (const IObject * _object) const = 0;
        virtual uint                        GetPropertyResourceVectorCount  (const IObject * _object) const = 0;
        virtual u8 *                        GetPropertyResourceVectorData   (const IObject * _object) const = 0;

        virtual IProperty::Callback         GetPropertyCallback             () const = 0;
    }; 
}