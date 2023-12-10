#pragma once

#include "IProperty.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Interface to Object descriptor
    //--------------------------------------------------------------------------------------
    class IObject;
    class IResource;

    class IClassDesc
    {
    public:
        using Func              = std::function<IObject*(const string &, class IObject *)>;
        using SingletonFunc     = std::function<IObject*()>;
        using ResizeVectorFunc  = std::function<void * (core::IObject *, core::uint, core::uint, core::uint &)>;

        enum class Flags : u64
        {
            None        = 0x0000000000000000,

            Singleton   = 0x0000000000000001,
            GameObject  = 0x0000000000000002,
            Component   = 0x0000000000000004,
            Model       = 0x0000000000000010,
            Instance    = 0x0000000000000020,
            Resource    = 0x0000000000000040,
            SceneNode   = 0x0000000000000080,
            Plugin      = 0x0000000000000100
        };
        virtual                             ~IClassDesc  () {}

        virtual bool                        IsRegisteredProperty        (const char * _propertyName) = 0;

        virtual void                        RegisterPropertyLayout      (const char * _className, IProperty::LayoutElementType _layoutElementType, const char * _label, IProperty::Flags _flags = IProperty::Flags::None) = 0;

        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, u8 * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, u16 * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, u32 * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, float * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, float3 * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, float4 * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, float4x4 * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, string * _offset, const char * _displayName, IProperty::Flags _flags) = 0;

        virtual void                        RegisterPropertyResource    (const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        RegisterPropertyResourcePtr (const char * _className, const char * _propertyName, IResource ** _offset, const char * _displayName, IProperty::Flags _flags) = 0;

        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, IProperty::Flags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, vector<IObject*>* _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, dictionary<IObject*>* _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) = 0;;
        
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, u32 _sizeOf, void * _offset, const char * _displayName = nullptr, IProperty::Flags _flags = IProperty::Flags::None) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, IProperty::Callback _funcPtr, const char * _displayName, IProperty::Flags _flags) = 0;

        virtual void                        RegisterEnum                (const char * _className, const char * _propertyName, u8 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u8 * _enumValues, IProperty::Flags _flags) = 0;
        virtual void                        RegisterEnum                (const char * _className, const char * _propertyName, u16 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u16 * _enumValues, IProperty::Flags _flags) = 0;
        virtual void                        RegisterEnum                (const char * _className, const char * _propertyName, u32 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u32 * _enumValues, IProperty::Flags _flags) = 0;

        virtual void                        RegisterEnumArray           (const char * _className, const char * _propertyName, float4 * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) = 0;
        virtual void                        RegisterEnumArray           (const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) = 0;
        virtual void                        RegisterEnumArray           (const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) = 0;
        virtual void                        RegisterEnumArray           (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, IProperty::Flags _flags, uint _enumSizeOf) = 0;

        virtual void                        RegisterResizeVectorFunc    (const char * _className, IClassDesc::ResizeVectorFunc _func) = 0;
        virtual void *                      ResizeVector                (core::IObject * _parent, core::uint _offset, core::uint _count, core::uint & _elementSize) const = 0;

        virtual const char *                GetClassName                () const = 0;
        virtual const char *                GetInterfaceName            () const = 0;
        virtual const char *                GetClassDisplayName         () const = 0;    
        virtual const char *                GetCategory                 () const = 0;
        virtual const char *                GetDescription              () const = 0;

        virtual void                        SetCategory                 (const char * _category) = 0;
        virtual void                        SetDescription              (const char * _description) = 0;

        virtual uint                        GetPropertyCount            () const = 0;
        virtual const IProperty *           GetPropertyByIndex          (uint _index) const = 0;
        virtual IProperty *                 GetPropertyByName           (const char * _propertyName) const = 0;
        virtual uint                        GetPropertyIndex            (const char * _propertyName) const = 0;
        virtual IProperty *                 GetPreviousProperty         (const char * _propertyName) const = 0;
        virtual IProperty *                 GetNextProperty             (const char * _propertyName) const = 0;
        virtual Func                        GetCreateFunc               () const = 0;
        virtual SingletonFunc               GetSingletonFunc            () const = 0;
        virtual IClassDesc::Flags           GetFlags                    () const = 0;
        virtual u32                         GetSizeOf                   () const = 0;
        virtual u32                         GetNextIndex                () const = 0;

        
 
    }; 
}