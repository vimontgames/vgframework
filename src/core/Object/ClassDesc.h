#pragma once

#include "core/IClassDesc.h"
#include "Property.h"

namespace vg::core
{
    struct ClassDesc final : public IClassDesc
    {
        ClassDesc();
        ~ClassDesc();

        bool                                IsRegisteredProperty        (const char * _propertyName) final override;

        void                                RegisterPropertyLayout      (const char * _className, PropertyLayoutElement _layoutElementType, const char * _label, bool * _offset, PropertyFlags _flags = PropertyFlags::None) final override;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, i8 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, i16 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, i32 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, i64 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, u8 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, u16 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, u32 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, u64 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, uint2 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, uint3 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, uint4 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, int2 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, int3 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, int4 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float2 * _offset, const char * _displayName, PropertyFlags _flags) final override; 
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float3 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float4 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float4x4 * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, string * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, BitMask * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, ObjectHandle * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, FloatCurveData * _offset, const char * _displayName, PropertyFlags _flags) final override;
        
        void                                RegisterPropertyResource    (const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterPropertyResourcePtr (const char * _className, const char * _propertyName, IResource ** _offset, const char * _displayName, PropertyFlags _flags) final override;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, PropertyFlags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, PropertyFlags _flags) final override;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, vector<IObject*>* _offset, const char * _elemType, const char * _displayName = nullptr, PropertyFlags _flags = PropertyFlags::None) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, dictionary<core::IObject*>* _offset, const char * _displayName = nullptr, PropertyFlags _flags = PropertyFlags::None) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, core::u32 _sizeOf, void * _offset, const char * _displayName = nullptr, PropertyFlags _flags = PropertyFlags::None) final override;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, IProperty::ActionCallback _funcPtr, const char * _displayName, PropertyFlags _flags) final override;

        void								RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, core::u8 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames,  const u8 * _enumValues, PropertyFlags _flags) final override;
        void								RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, core::u16 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u16 * _enumValues, PropertyFlags _flags) final override;
        void								RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, core::u32 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u32 * _enumValues, PropertyFlags _flags) final override;
        void								RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, core::u64 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u64 * _enumValues, PropertyFlags _flags) final override;
        
        void								RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, core::i8 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames,  const i8 * _enumValues, PropertyFlags _flags) final override;
        void								RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, core::i16 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const i16 * _enumValues, PropertyFlags _flags) final override;
        void								RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, core::i32 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const i32 * _enumValues, PropertyFlags _flags) final override;
        void								RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, core::i64 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const i64 * _enumValues, PropertyFlags _flags) final override;

        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) final override;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, core::string * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) final override;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) final override;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, core::float4 * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) final override;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) final override;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) final override;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) final override;

        void                                RegisterResizeVectorFunc    (const char * _className, IClassDesc::ResizeVectorFunc _func) final override;
        void *                              ResizeVector                (core::IObject * _parent, core::uint _offset, core::uint _count) const final override;

        const char *                        GetClassName                () const final override;
        const char *                        GetParentClassName          () const final override;
        const char *                        GetClassDisplayName         () const final override;
        const char *                        GetCategory                 () const final override;
        const char *                        GetDescription              () const final override;
        const char *                        GetIcon                     () const final override;
        Priority                            GetPriority                 () const final override;

        void                                SetCategory                 (const char * _category) final override;
        void                                SetDescription              (const char * _description) final override;
        void                                SetIcon                     (const char * _icon) final override;
        void                                SetPriority                 (Priority _priority) final override;
        void                                SetFlags                    (ClassDescFlags _flags) final override;

        uint                                GetPropertyCount            () const final override;
        const IProperty *                   GetPropertyByIndex          (uint _index) const final override;
        const IProperty *                   GetAliasedProperty          (const IProperty * _prop) const final override;
        IProperty *                         GetPropertyByName           (const char * _propertyName) const final override;
        IProperty *                         GetLastPropertyByName       (const char * _propertyName) const final override;
        uint                                GetPropertyIndex            (const char * _propertyName) const final override;
        uint                                GetLastPropertyIndex        (const char * _propertyName) const final override;
        IProperty *                         GetPreviousProperty         (const char * _propertyName) const final override;
        IProperty *                         GetNextProperty             (const char * _propertyName) const final override;

        IClassDesc::Func                    GetCreateFunc               () const final override;
        IClassDesc::SingletonFunc           GetSingletonFunc            () const final override;
        ClassDescFlags                      GetFlags                    () const final override;
        u32                                 GetSizeOf                   () const final override;
        u32                                 GetNextIndex                () const final override;

        bool                                pushProperty                (const Property & _prop);

        const char *                        name                        = nullptr;
        const char *                        parentClassName             = nullptr;
        const char *                        displayName                 = nullptr;
        const char *                        category                    = nullptr;
        const char *                        description                 = nullptr;
        const char *                        icon                        = nullptr;
        ClassDescFlags                      flags                       = ClassDescFlags::None;
        u32                                 sizeOf                      = 0;
        Priority                            priority                    = 0;
        IClassDesc::Func                    createFunc                  = nullptr;
        IClassDesc::SingletonFunc           createSingletonFunc         = nullptr;
        IClassDesc::ResizeVectorFunc        resizeVectorFunc            = nullptr;

        vector<Property>                    properties;
        mutable u32                         count = 0;

    private:
        template <typename T> void          registerClassMemberT        (const char * _className, const char * _propertyName, T * _offset, const char * _displayName, PropertyFlags _flags);
        template <typename T> void          registerEnumArrayT          (const char * _className, const char * _propertyName, T * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf);

    };
}