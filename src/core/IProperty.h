#pragma once

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Interface to PropertyDescriptor
    //--------------------------------------------------------------------------------------

    class IProperty
    {
    public:

        using Func = bool (__cdecl*)(class IObject*);

        enum class Type : u32
        {
            Undefined = 0,
            Bool,
            Uint16,
            Uint32,
            Float,
            Float4,
            Matrix44,
            String,
            ObjectPointer,
            Resource,
            ResourceVector,
            Function,
            EnumU8,
            EnumU16,
            EnumU32,
            EnumFlagsU8,
            EnumFlagsU16,
            EnumFlagsU32,
            ObjectVector,
            ObjectPointerVector,
            ObjectPointerDictionary
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

        virtual void                        setRange                (float2 _range) = 0;

        virtual const char *                getName                 () const = 0;
        virtual const char *                getClassName            () const = 0;
        virtual const char *                getDisplayName          () const = 0;
        virtual Type                        getType                 () const = 0;
        virtual Flags                       getFlags                () const = 0;
        virtual uint_ptr                    getOffset               () const = 0;
		virtual u32					        getValue				() const = 0;
        virtual float2                      getRange                () const = 0;
        virtual u32                         getEnumCount            () const = 0;
        virtual const char *                getEnumName             (uint index) const = 0;
        virtual u32                         getEnumValue            (uint index) const = 0;
    }; 
}