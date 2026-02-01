#pragma once

#include "IProperty.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Interface to Object descriptor
    //--------------------------------------------------------------------------------------
    class IObject;
    class ObjectHandle;
    class IResource;
    class BitMask;
    struct FloatCurveData;

    enum class ClassDescFlags : u64
    {
        None            = 0x0000000000000000,

        Singleton       = 0x0000000000000001,
        GameObject      = 0x0000000000000002,
        Component       = 0x0000000000000004,
        Model           = 0x0000000000000010,
        Instance        = 0x0000000000000020,
        Resource        = 0x0000000000000040,
        SceneNode       = 0x0000000000000080,
        Plugin          = 0x0000000000000100,
        UID             = 0x0000000000000200,
        Abstract        = 0x0000000000000400,
        Hidden          = 0x0000000000004000
    };

    using Priority = core::u16;
    using ClassCRC = core::u64;

    class IClassDesc
    {
    public:

        using Func              = std::function<IObject*(const string &, class IObject *)>;
        using SingletonFunc     = std::function<IObject*()>;
        using ResizeVectorFunc  = std::function<void * (core::IObject *, core::uint, core::uint)>;

        virtual                             ~IClassDesc  () {}

        virtual bool                        IsRegisteredProperty        (const char * _propertyName) = 0;

        virtual void                        RegisterPropertyLayout      (const char * _className, PropertyLayoutElement _layoutElementType, const char * _label, bool * _offset, PropertyFlags _flags = PropertyFlags::None) = 0;

        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, i8 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, i16 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, i32 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, i64 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, u8 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, u16 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, u32 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, u64 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, uint2 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, uint3 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, uint4 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, int2 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, int3 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, int4 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, float * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, float2 * _offset, const char * _displayName, PropertyFlags _flags) = 0; 
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, float3 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, float4 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, float4x4 * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, string * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, BitMask * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, ObjectHandle * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, FloatCurveData * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        
        virtual void                        RegisterPropertyResource    (const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterPropertyResourcePtr (const char * _className, const char * _propertyName, IResource ** _offset, const char * _displayName, PropertyFlags _flags) = 0;

        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, PropertyFlags _flags) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName = nullptr, PropertyFlags _flags = PropertyFlags::None) = 0;
        
        template <class T> void             RegisterPropertyT           (const char * _className, const char * _propertyName, vector<T *> * _offset, const char * _displayName = nullptr, PropertyFlags _flags = PropertyFlags::None)
        {
            RegisterProperty(_className, _propertyName, (vector<IObject *>*)_offset, T::getStaticClassName(), _displayName, _flags);
        }
        
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, vector<IObject*>* _offset, const char * _elemType, const char * _displayName = nullptr, PropertyFlags _flags = PropertyFlags::None) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, dictionary<IObject*>* _offset, const char * _displayName = nullptr, PropertyFlags _flags = PropertyFlags::None) = 0;;
        
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, u32 _sizeOf, void * _offset, const char * _displayName = nullptr, PropertyFlags _flags = PropertyFlags::None) = 0;
        virtual void                        RegisterProperty            (const char * _className, const char * _propertyName, IProperty::ActionCallback _funcPtr, const char * _displayName, PropertyFlags _flags) = 0;

        virtual void                        RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, u8 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames,  const u8 * _enumValues, PropertyFlags _flags) = 0;
        virtual void                        RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, u16 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u16 * _enumValues, PropertyFlags _flags) = 0;
        virtual void                        RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, u32 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u32 * _enumValues, PropertyFlags _flags) = 0;
        virtual void                        RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, u64 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const u64 * _enumValues, PropertyFlags _flags) = 0;
        
        virtual void                        RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, i8 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames,  const i8 * _enumValues, PropertyFlags _flags) = 0;
        virtual void                        RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, i16 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const i16 * _enumValues, PropertyFlags _flags) = 0;
        virtual void                        RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, i32 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const i32 * _enumValues, PropertyFlags _flags) = 0;
        virtual void                        RegisterEnum                (const char * _className, const char * _propertyName, const char * _enumTypeName, i64 * _offset, const char * _displayName, uint _enumCount, const char * _enumNames, const i64 * _enumValues, PropertyFlags _flags) = 0;

        virtual void                        RegisterEnumArray           (const char * _className, const char * _propertyName, bool * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) = 0;
        virtual void                        RegisterEnumArray           (const char * _className, const char * _propertyName, core::string * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) = 0;
        virtual void                        RegisterEnumArray           (const char * _className, const char * _propertyName, core::u8 * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) = 0;
        virtual void                        RegisterEnumArray           (const char * _className, const char * _propertyName, float4 * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) = 0;
        virtual void                        RegisterEnumArray           (const char * _className, const char * _propertyName, IResource * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) = 0;
        virtual void                        RegisterEnumArray           (const char * _className, const char * _propertyName, IObject * _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) = 0;
        virtual void                        RegisterEnumArray           (const char * _className, const char * _propertyName, IObject ** _offset, const char * _displayName, uint _enumCount, uint _elementSize, const char * _enumNames, const void * _enumValues, PropertyFlags _flags, uint _enumSizeOf) = 0;
        
        virtual void                        RegisterResizeVectorFunc    (const char * _className, IClassDesc::ResizeVectorFunc _func) = 0;
        virtual void *                      ResizeVector                (core::IObject * _parent, core::uint _offset, core::uint _count) const = 0;

        virtual const char *                GetClassName                () const = 0;
        virtual ClassCRC                    GetClassCRC                 () const = 0;
        virtual const char *                GetParentClassName          () const = 0;
        virtual ClassCRC                    GetParentClassCRC           () const = 0;
        virtual const char *                GetClassDisplayName         () const = 0;    
        virtual const char *                GetCategory                 () const = 0;
        virtual const char *                GetDescription              () const = 0;
        virtual const char *                GetIcon                     () const = 0;
        virtual Priority                    GetPriority                 () const = 0;

        virtual void                        SetCategory                 (const char * _category) = 0;
        virtual void                        SetDescription              (const char * _description) = 0;
        virtual void                        SetIcon                     (const char * _icon) = 0;
        virtual void                        SetPriority                 (Priority _priority) = 0;
        virtual void                        SetFlags                    (ClassDescFlags _flags) = 0;

        virtual uint                        GetPropertyCount            () const = 0;
        virtual const IProperty *           GetPropertyByIndex          (uint _index) const = 0;
        virtual const IProperty *           GetAliasedProperty          (const IProperty * _prop) const = 0;
        virtual IProperty *                 GetPropertyByName           (const char * _propertyName, bool _warning = true) const = 0;
        virtual IProperty *                 GetLastPropertyByName       (const char * _propertyName, bool _warning = true) const = 0;
        virtual uint                        GetPropertyIndex            (const char * _propertyName, bool _warning = true) const = 0;
        virtual uint                        GetLastPropertyIndex        (const char * _propertyName, bool _warning = true) const = 0;
        virtual IProperty *                 GetPreviousProperty         (const char * _propertyName, bool _warning = true) const = 0;
        virtual IProperty *                 GetNextProperty             (const char * _propertyName, bool _warning = true) const = 0;
    
        virtual Func                        GetCreateFunc               () const = 0;
        virtual SingletonFunc               GetSingletonFunc            () const = 0;
        virtual ClassDescFlags              GetFlags                    () const = 0;
        virtual u32                         GetSizeOf                   () const = 0;
        virtual u32                         GetNextIndex                () const = 0;       
 
    }; 
}