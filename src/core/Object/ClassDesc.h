#pragma once

#include "core/IClassDesc.h"
#include "Property.h"

namespace vg::core
{
    struct ClassDesc : public IClassDesc
    {
        bool                                isRegisteredProperty(const char * _propertyName) final;

        void                                registerProperty    (const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, core::u16 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, core::u32 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, float * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, float3 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, float4 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, float4x4 * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, string * _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, IResource ** _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, IProperty::Callback _funcPtr, const char * _displayName, IProperty::Flags _flags) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, core::u32 _sizeOf, void * _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, vector<IObject*>* _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) final;
        void                                registerProperty    (const char * _className, const char * _propertyName, dictionary<core::IObject*>* _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) final;
        
        void								registerEnum        (const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u8 * _enumValues, IProperty::Flags _flags) final;
        void								registerEnum        (const char * _className, const char * _propertyName, core::u16 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u16 * _enumValues, IProperty::Flags _flags) final;
        void								registerEnum        (const char * _className, const char * _propertyName, core::u32 * _offset, const char * _displayName, u32 _enumCount, const char * _enumNames, const u32 * _enumValues, IProperty::Flags _flags) final;

        void                                registerEnumArray   (const char * _className, const char * _propertyName, core::float4 * _offset, const char * _displayName, uint _enumCount, uint _enumSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags) final;
        void                                registerEnumArray   (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, uint _enumCount, uint _enumSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags) final;

        const char *                        getClassName        () const final;
        const char *                        getClassDisplayName () const final;
        uint                                getPropertyCount    () const final;
        const IProperty *                   getPropertyByIndex  (uint _index) const final;
        IProperty *                         getPropertyByName   (const char * _propertyName) const final;
        IClassDesc::Func                    getCreateFunc       () const final;
        IClassDesc::SingletonFunc           getSingletonFunc    () const final;
        IClassDesc::Flags                   getFlags            () const final;
        u32                                 getSizeOf           () const final;
        u32                                 getNextIndex        () const final;

        const char *                        name = nullptr;
        const char *                        displayName = nullptr;
        IClassDesc::Flags                   flags = IClassDesc::Flags::None;
        u32                                 sizeOf = 0;
        IClassDesc::Func                    createFunc;
        IClassDesc::SingletonFunc           createSingletonFunc;
        vector<Property>                    properties;

        mutable u32                         count = 0;

    private:
        template <typename T> void  registerClassMemberT    (const char * _className, const char * _propertyName, T * _offset, const char * _displayName, IProperty::Flags _flags);
        template <typename T> void  registerEnumArrayT      (const char * _className, const char * _propertyName, T * _offset, const char * _displayName, uint _enumCount, uint _enumSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags);

    };
}