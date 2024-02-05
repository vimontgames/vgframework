#pragma once

#include "core/IClassDesc.h"
#include "Property.h"

namespace vg::core
{
    struct ClassDesc final : public IClassDesc
    {
        bool                                IsRegisteredProperty        (const char * _propertyName) final override;

        void                                RegisterPropertyLayout      (const char * _className, IProperty::LayoutElementType _layoutElementType, const char * _label, bool * _offset, IProperty::Flags _flags = IProperty::Flags::None) final override;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, core::u16 * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, core::u32 * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, uint2 * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, uint3 * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, uint4 * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float2 * _offset, const char * _displayName, IProperty::Flags _flags) final override; 
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float3 * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float4 * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float4x4 * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, string * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, BitMask * _offset, const char * _displayName, IProperty::Flags _flags) final override;

        void                                RegisterPropertyResource    (const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterPropertyResourcePtr (const char * _className, const char * _propertyName, IResource ** _offset, const char * _displayName, IProperty::Flags _flags) final override;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, IProperty::Flags _flags) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, IProperty::Flags _flags) final override;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, vector<IObject*>* _offset, const char * _elemType, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, dictionary<core::IObject*>* _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) final override;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, core::u32 _sizeOf, void * _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) final override;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, IProperty::Callback _funcPtr, const char * _displayName, IProperty::Flags _flags) final override;

        void								RegisterEnum                (const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u8 * _enumValues, IProperty::Flags _flags) final override;
        void								RegisterEnum                (const char * _className, const char * _propertyName, core::u16 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u16 * _enumValues, IProperty::Flags _flags) final override;
        void								RegisterEnum                (const char * _className, const char * _propertyName, core::u32 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u32 * _enumValues, IProperty::Flags _flags) final override;

        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, core::float4 * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) final override;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) final override;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) final override;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) final override;

        void                                RegisterResizeVectorFunc    (const char * _className, IClassDesc::ResizeVectorFunc _func) final override;
        void *                              ResizeVector                (core::IObject * _parent, core::uint _offset, core::uint _count, core::uint & _elementSize) const final override;

        const char *                        GetClassName                () const final override;
        const char *                        GetParentClassName          () const final override;
        const char *                        GetClassDisplayName         () const final override;
        const char *                        GetCategory                 () const final override;
        const char *                        GetDescription              () const final override;
        const char *                        GetIcon                     () const final override;

        void                                SetCategory                 (const char * _category) final override;
        void                                SetDescription              (const char * _description) final override;
        void                                SetIcon                     (const char * _icon) final override;

        uint                                GetPropertyCount            () const final override;
        const IProperty *                   GetPropertyByIndex          (uint _index) const final override;
        IProperty *                         GetPropertyByName           (const char * _propertyName) const final override;
        uint                                GetPropertyIndex            (const char * _propertyName) const final override;
        IProperty *                         GetPreviousProperty         (const char * _propertyName) const final override;
        IProperty *                         GetNextProperty             (const char * _propertyName) const final override;
        IClassDesc::Func                    GetCreateFunc               () const final override;
        IClassDesc::SingletonFunc           GetSingletonFunc            () const final override;
        IClassDesc::Flags                   GetFlags                    () const final override;
        u32                                 GetSizeOf                   () const final override;
        u32                                 GetNextIndex                () const final override;

        const char *                        name                        = nullptr;
        const char *                        parentClassName             = nullptr;
        const char *                        displayName                 = nullptr;
        const char *                        category                    = nullptr;
        const char *                        description                 = nullptr;
        const char *                        icon                        = nullptr;
        IClassDesc::Flags                   flags                       = IClassDesc::Flags::None;
        u32                                 sizeOf                      = 0;
        IClassDesc::Func                    createFunc                  = nullptr;
        IClassDesc::SingletonFunc           createSingletonFunc         = nullptr;
        IClassDesc::ResizeVectorFunc        resizeVectorFunc            = nullptr;
        vector<Property>                    properties;

        mutable u32                         count = 0;

    private:
        template <typename T> void          registerClassMemberT        (const char * _className, const char * _propertyName, T * _offset, const char * _displayName, IProperty::Flags _flags);
        template <typename T> void          registerEnumArrayT          (const char * _className, const char * _propertyName, T * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf);

    };
}