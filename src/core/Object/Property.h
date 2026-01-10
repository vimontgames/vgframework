#pragma once

#include "core/IClassDesc.h"

namespace vg::core
{
    struct EnumDesc 
    {
        string name;

        union MyUnion
        {
            u64 u = 0;
            i64 s;
        } value;

        EnumValueFlags flags = (EnumValueFlags)0x0;
    };

    class Property final : public IProperty
    {
    public:
                                        Property                        (const char * _class = "", const char * _name = "", PropertyType _type = PropertyType::Undefined, uint_ptr _offset = 0, core::u32 _sizeOf = 0, const char * _prettyName = "", PropertyFlags _flags = (PropertyFlags)0x0, uint _enumCount = 0, const char * _enumNames = nullptr, const void * _enumValues = nullptr, uint _enumSizeOf = -1);
                                        Property                        (const Property & _other);
                                        ~Property                       ();

        void                            SetInterface                    (const char * _interface) final override;
        void                            SetRange                        (float2 _range) final override;
        void                            SetPropertyRangeCallback        (PropertyRangeCallback _func) final override;
        void                            SetPropertyHiddenCallback       (IsPropertyHiddenCallback _func) final override;
        void                            SetPropertyReadOnlyCallback     (IsPropertyReadOnlyCallback _func) final override;
        void                            SetPropertyNamesCallback        (PropertyNamesCallback _func) final override;
        void                            SetDefaultFolder                (const char * _path) final override;
        void                            SetFlags                        (PropertyFlags _flagsToSet, PropertyFlags _flagsToRemove = PropertyFlags::None) final override;
        void                            SetOffset                       (uint_ptr _offset) final override;
        void                            SetDescription                  (const char * _description) final override;
        void                            SetEnumTypeName                 (const char * _enumTypeName) final override;
        bool                            SetEnumValueFlags               (u64 _value, EnumValueFlags _flags, bool _enabled) final override;
        void                            SetDisplayName                  (const char * _displayName) final override;
        EnumValueFlags                  GetUnsignedEnumValueFlags       (u64 _value) const final override;
        EnumValueFlags                  GetSignedEnumValueFlags         (i64 _value) const final override;
        const char *                    GetInterface                    () const final override;
        const char *                    GetName                         () const final override;
        const char *                    GetClassName                    () const final override;
        const char *                    GetDefaultFolder                () const final override;
        const char *                    GetDescription                  () const final override;
        PropertyType                    GetType                         () const final override;
        uint_ptr                        GetOffset                       () const final override;
		core::u32		                GetSizeOf		                () const final override;
        const char *                    GetDisplayName                  () const final override;
        PropertyFlags                   GetFlags                        () const final override;
        float2                          GetRange                        (const IObject * _object, core::uint _index = 0) const final override;
        bool                            IsHidden                        (const IObject * _object, core::uint _index = 0) const final override;
        bool                            IsReadOnly                      (const IObject * _object, core::uint _index = 0) const final override;
        core::vector<core::string>      GetPropertyNames                (const IObject * _object, core::uint _index = 0) const final override;
        const char *                    GetEnumTypeName                 () const final override;
        u32                             GetEnumCount                    () const final override;
        void                            SetEnumName                     (uint index, core::string _name) final override;
        const core::string &            GetEnumName                     (uint index) const final override;
        u64                             GetUnsignedEnumValue            (uint index) const final override;
        i64                             GetSignedEnumValue              (uint index) const final override;

        bool *                          GetPropertyBool                 (const IObject * _object, uint _index = 0) const final override;

        i8 *                            GetPropertyInt8                 (const IObject * _object) const final override;
        i16 *                           GetPropertyInt16                (const IObject * _object) const final override;
        i32 *                           GetPropertyInt32                (const IObject * _object) const final override;
        i64 *                           GetPropertyInt64                (const IObject * _object) const final override;
        
        u8 *                            GetPropertyUint8                (const IObject * _object, uint _index = 0) const final override;
        u16 *                           GetPropertyUint16               (const IObject * _object) const final override;
        u32 *                           GetPropertyUint32               (const IObject * _object) const final override;
        u64 *                           GetPropertyUint64               (const IObject * _object) const final override;
        
        i32 *                           GetPropertyIntN                 (const IObject * _object, uint _componentCount, uint _index = 0) const override;
        u32 *                           GetPropertyUintN                (const IObject * _object, uint _componentCount, uint _index = 0) const final override;
        
        FloatCurveData *                GetPropertyFloatCurveData       (const IObject * _object) const final override;
        
        BitMask *                       GetPropertyBitMask              (const IObject * _object) const final override;
        ObjectHandle *                  GetPropertyObjectHandle         (const IObject * _object) const final override;

        float *                         GetPropertyFloat                (const IObject * _object) const final override;
        float2 *                        GetPropertyFloat2               (const IObject * _object) const final override;
        float3 *                        GetPropertyFloat3               (const IObject * _object) const final override;
        float4 *                        GetPropertyFloat4               (const IObject * _object, uint _index = 0) const final;
        float *                         GetPropertyFloatN               (const IObject * _object, uint _componentCount, uint _index = 0) const final override;
        float4x4 *                      GetPropertyFloat4x4             (const IObject * _object) const final override;
        string *                        GetPropertyString               (const IObject * _object, uint _index = 0) const final override;
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

        IProperty::ActionCallback       GetPropertyActionCallback             () const final override;
        PropertyLayoutElement           GetLayoutElementType            () const final override;

        bool                            IsResourceProperty              () const final override;
        bool                            IsObjectProperty                () const final override;

    protected:
        template <typename T> void      initEnum                        (uint _enumCount, const char * _enumNames, const void * _enumValues);
        void                            checkPropertyType               (PropertyType _type) const;
        
    private:
        const char *                    name                = nullptr;
        const char *                    className           = nullptr;
        const char *                    displayName         = nullptr;
        const char *                    defaultFolder       = nullptr;
        const char *                    interfaceType       = nullptr;
        const char *                    description         = nullptr;
        const char *                    enumTypeName        = nullptr;
        PropertyType                    type			    = PropertyType::Undefined;
        uint_ptr	                    offset			    = (uint_ptr)-1;
		core::u32	                    sizeOf			    = 0x0;
        PropertyFlags                   flags			    = PropertyFlags::None;
        float2		                    range			    = float2(0.0f, 0.0f);
        PropertyRangeCallback           rangeCallback       = nullptr;
        IsPropertyHiddenCallback        isHiddenCallback    = nullptr;
        IsPropertyReadOnlyCallback      isReadOnlyCallback  = nullptr;
        PropertyNamesCallback           namesCallback       = nullptr;
        vector<EnumDesc>                enums;

        // Do not forget to update Property(const Property & _other) when you add a member variable here!
    };   
}