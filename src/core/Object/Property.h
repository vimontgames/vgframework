#pragma once

#include "core/IClassDesc.h"

namespace vg::core
{
    struct EnumDesc 
    {
        string name;
        uint value = 0x0;
    };

    class Property : public IProperty
    {
    public:
                                    Property                        (const char * _class, const char * _name, Type _type, uint_ptr _offset, core::u32 _sizeOf, const char * _prettyName, Flags _flags, uint _enumCount = 0, const char * _enumNames = nullptr, const void * _enumValues = nullptr);
                                    ~Property                       ();

        void                        setRange                        (float2 _range) final;
        void                        setFlags                        (Flags _flagsToSet, Flags _flagsToRemove = Flags::None) final;

        const char *                getName                         () const final;
        const char *                getClassName                    () const final;
        Type                        getType                         () const final;
        uint_ptr                    getOffset                       () const final;
		core::u32		            getSizeOf		                () const final;
        const char *                getDisplayName                  () const final;
        Flags                       getFlags                        () const final; 
        float2                      getRange                        () const final;
        u32                         getEnumCount                    () const final;
        const char *                getEnumName                     (uint index) const final;
        u32                         getEnumValue                    (uint index) const final;

        bool *                      GetPropertyBool                 (const IObject * _object) const final;
        u8 *                        GetPropertyUint8                (const IObject * _object) const final;
        u16 *                       GetPropertyUint16               (const IObject * _object) const final;
        u32 *                       GetPropertyUint32               (const IObject * _object) const final;
        u64 *                       GetPropertyUint64               (const IObject * _object) const final;
        float *                     GetPropertyFloat                (const IObject * _object) const final;
        float4 *                    GetPropertyFloat4               (const IObject * _object) const final;
        float4x4 *                  GetPropertyFloat4x4             (const IObject * _object) const final;
        string *                    GetPropertyString               (const IObject * _object) const final;
        IResource *                 GetPropertyResource             (const IObject * _object) const final;
        IObject *                   GetPropertyObjectRef            (const IObject * _object) const final;
        vector<IObject*> *          GetPropertyObjectRefVector      (const IObject * _object) const final;
        dictionary<IObject*> *      GetPropertyObjectRefDictionary  (const IObject * _object) const final;

        uint                        GetPropertyObjectVectorCount    (const IObject * _object) const final;
        u8 *                        GetPropertyObjectVectorData     (const IObject * _object) const final;
        uint                        GetPropertyResourceVectorCount  (const IObject * _object) const final;
        u8 *                        GetPropertyResourceVectorData   (const IObject * _object) const final;

        IProperty::Callback         GetPropertyCallback             () const final;

    protected:
        template <typename T> void  initEnum                        (uint _enumCount, const char * _enumNames, const void * _enumValues);

    private:
        const char *                name            = nullptr;
        const char *                className       = nullptr;
        const char *                displayName     = nullptr;
        Type		                type			= Type::Undefined;
        uint_ptr	                offset			= (uint_ptr)-1;
		core::u32	                sizeOf			= 0x0;
        Flags		                flags			= Flags::None;
        float2		                range			= float2(0.0f, 0.0f);
        vector<EnumDesc>            enums;
    };   
}