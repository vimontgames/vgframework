#pragma once

#include "core/IClassDesc.h"
#include "Property.h"

namespace vg::core
{
    struct ClassDesc : public IClassDesc
    {
        bool                                IsRegisteredProperty        (const char * _propertyName) final;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, core::u16 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, core::u32 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float3 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float4 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, float4x4 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, string * _offset, const char * _displayName, IProperty::Flags _flags) final;

        void                                RegisterPropertyResource    (const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                RegisterPropertyResourcePtr (const char * _className, const char * _propertyName, IResource ** _offset, const char * _displayName, IProperty::Flags _flags) final;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, IProperty::Flags _flags) final;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, vector<IObject*>* _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) final;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, dictionary<core::IObject*>* _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) final;
        void                                RegisterProperty            (const char * _className, const char * _propertyName, core::u32 _sizeOf, void * _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) final;

        void                                RegisterProperty            (const char * _className, const char * _propertyName, IProperty::Callback _funcPtr, const char * _displayName, IProperty::Flags _flags) final;

        void								RegisterEnum                (const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u8 * _enumValues, IProperty::Flags _flags) final;
        void								RegisterEnum                (const char * _className, const char * _propertyName, core::u16 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u16 * _enumValues, IProperty::Flags _flags) final;
        void								RegisterEnum                (const char * _className, const char * _propertyName, core::u32 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u32 * _enumValues, IProperty::Flags _flags) final;

        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, core::float4 * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) final;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) final;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) final;
        void                                RegisterEnumArray           (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) final;

        void                                RegisterResizeVectorFunc    (const char * _className, IClassDesc::ResizeVectorFunc _func) final override;
        void *                              ResizeVector                (core::IObject * _parent, core::uint _offset, core::uint _count, core::uint & _elementSize) const final override;

        const char *                        GetClassName                () const final;
        const char *                        GetClassDisplayName         () const final;
        uint                                GetPropertyCount            () const final;
        const IProperty *                   GetPropertyByIndex          (uint _index) const final;
        IProperty *                         GetPropertyByName           (const char * _propertyName) const final;
        IClassDesc::Func                    GetCreateFunc               () const final;
        IClassDesc::SingletonFunc           GetSingletonFunc            () const final;
        IClassDesc::Flags                   GetFlags                    () const final;
        u32                                 GetSizeOf                   () const final;
        u32                                 GetNextIndex                () const final;

        const char *                        name                        = nullptr;
        const char *                        displayName                 = nullptr;
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