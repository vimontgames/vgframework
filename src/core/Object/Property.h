#pragma once

#include "core/IClassDesc.h"

namespace vg::core
{
    struct EnumDesc 
    {
        string name;
        u64 value = 0x0;
    };

    class Property final : public IProperty
    {
    public:
                                        Property                        (const char * _class, const char * _name, Type _type, uint_ptr _offset, core::u32 _sizeOf, const char * _prettyName, Flags _flags, uint _enumCount = 0, const char * _enumNames = nullptr, const void * _enumValues = nullptr, uint _enumSizeOf = -1);
                                        ~Property                       ();

        void                            setRange                        (float2 _range) final override;
        void                            setDefaultFolder                (const char * _path) final override;
        void                            setFlags                        (Flags _flagsToSet, Flags _flagsToRemove = Flags::None) final;

        const char *                    getName                         () const final override;
        const char *                    getClassName                    () const final override;
        const char *                    getDefaultFolder                () const final override;
        Type                            getType                         () const final override;
        uint_ptr                        getOffset                       () const final override;
		core::u32		                getSizeOf		                () const final override;
        const char *                    getDisplayName                  () const final override;
        Flags                           getFlags                        () const final override; 
        float2                          getRange                        () const final override;
        u32                             getEnumCount                    () const final override;
        const char *                    getEnumName                     (uint index) const final override;
        u64                             getEnumValue                    (uint index) const final override;

        bool *                          GetPropertyBool                 (const IObject * _object) const final override;

        i8 *                            GetPropertyInt8                 (const IObject * _object) const final override;
        i16 *                           GetPropertyInt16                (const IObject * _object) const final override;
        i32 *                           GetPropertyInt32                (const IObject * _object) const final override;
        i64 *                           GetPropertyInt64                (const IObject * _object) const final override;

        u8 *                            GetPropertyUint8                (const IObject * _object) const final override;
        u16 *                           GetPropertyUint16               (const IObject * _object) const final override;
        u32 *                           GetPropertyUint32               (const IObject * _object) const final override;
        u32 *                           GetPropertyUintN                (const IObject * _object, uint _componentCount, uint _index = 0) const final override;
        u64 *                           GetPropertyUint64               (const IObject * _object) const final override;

        float *                         GetPropertyFloat                (const IObject * _object) const final override;
        float2 *                        GetPropertyFloat2               (const IObject * _object) const final override;
        float3 *                        GetPropertyFloat3               (const IObject * _object) const final override;
        float4 *                        GetPropertyFloat4               (const IObject * _object, uint _index = 0) const final;
        float *                         GetPropertyFloatN               (const IObject * _object, uint _componentCount, uint _index = 0) const final override;
        float4x4 *                      GetPropertyFloat4x4             (const IObject * _object) const final override;
        string *                        GetPropertyString               (const IObject * _object) const final override;
        IResource *                     GetPropertyResource             (const IObject * _object, uint _index = 0) const final override;
        IResource **                    GetPropertyResourcePtr          (const IObject * _object, uint _index = 0) const final override;
        vector<IResource *> *           GetPropertyResourcePtrVector    (const IObject * _object) const final override;
        IObject *                       GetPropertyObject               (const IObject * _object, uint _index = 0) const final override;
        IObject **                      GetPropertyObjectPtr            (const IObject * _object, uint _index = 0) const final override;
        vector<IObject*> *              GetPropertyObjectPtrVector      (const IObject * _object) const final override;
        dictionary<IObject*> *          GetPropertyObjectPtrDictionary  (const IObject * _object) const final override;

        uint                            GetPropertyObjectVectorCount    (const IObject * _object) const final override;
        u8 *                            GetPropertyObjectVectorData     (const IObject * _object) const final override;
        IObject *                       GetPropertyObjectVectorElement  (const IObject * _object, uint _index) const final override;

        uint                            GetPropertyResourceVectorCount  (const IObject * _object) const final override;
        u8 *                            GetPropertyResourceVectorData   (const IObject * _object) const final override;
        IResource *                     GetPropertyResourceVectorElement(const IObject * _object, uint _index) const final override;

        IProperty::Callback             GetPropertyCallback             () const final override;

        IProperty::LayoutElementType    GetLayoutElementType            () const final override;

    protected:
        template <typename T> void      initEnum                        (uint _enumCount, const char * _enumNames, const void * _enumValues);
        void                            checkPropertyType               (Type _type) const;
        
    private:
        const char *                    name            = nullptr;
        const char *                    className       = nullptr;
        const char *                    displayName     = nullptr;
        const char *                    defaultFolder   = nullptr;
        Type		                    type			= Type::Undefined;
        uint_ptr	                    offset			= (uint_ptr)-1;
		core::u32	                    sizeOf			= 0x0;
        Flags		                    flags			= Flags::None;
        float2		                    range			= float2(0.0f, 0.0f);
        vector<EnumDesc>                enums;
    };   
}