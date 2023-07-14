#pragma once

#include "IProperty.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Interface to Object descriptor
    //--------------------------------------------------------------------------------------

    class IClassDesc
    {
    public:
        using Func          = std::function<IObject*(const string &, class IObject *)>;
        using SingletonFunc = std::function<IObject*()>;

        enum class Flags : u64
        {
            None        = 0x0000000000000000,

            Singleton   = 0x0000000000000001,
            GameObject  = 0x0000000000000002,
            Component   = 0x0000000000000004,
            Model       = 0x0000000000000008,
            Instance    = 0x0000000000000010,
            Resource    = 0x0000000000000020,
            SceneNode   = 0x0000000000000040
        };
        virtual                             ~IClassDesc  () {}

        virtual bool                        isRegisteredProperty    (const char * _propertyName) = 0;

        virtual void                        registerProperty        (const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, core::u32 * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, core::u16 * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, float * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, float4 * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, float4x4 * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, string * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, class IResource ** _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, class IObject ** _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, IProperty::Callback _funcPtr, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, core::u32 _sizeOf, void * _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, vector<IObject*>* _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) = 0;
        virtual void                        registerProperty        (const char * _className, const char * _propertyName, core::dictionary<core::IObject*>* _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) = 0;;
        
        virtual void                        registerEnum            (const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u8 * _enumValues, IProperty::Flags _flags) = 0;
        virtual void                        registerEnum            (const char * _className, const char * _propertyName, core::u16 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u16 * _enumValues, IProperty::Flags _flags) = 0;
        virtual void                        registerEnum            (const char * _className, const char * _propertyName, core::u32 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u32 * _enumValues, IProperty::Flags _flags) = 0;

        virtual const char *                getClassName            () const = 0;
        virtual const char *                getClassDisplayName     () const = 0;
        virtual uint                        getPropertyCount        () const = 0;
        virtual const IProperty *           getPropertyByIndex      (uint _index) const = 0;
        virtual IProperty *                 getPropertyByName       (const char * _propertyName) const = 0;
        virtual Func                        getCreateFunc           () const = 0;
        virtual SingletonFunc               getSingletonFunc        () const = 0;
        virtual IClassDesc::Flags           getFlags                () const = 0;
        virtual u32                         getSizeOf               () const = 0;
        virtual u32                         getNextIndex            () const = 0;
    }; 
}