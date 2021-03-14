#pragma once

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Use these helpers to register classes and class members (MyClass must inherit from core::IObject)
    //
    // i.e.: 
    // static bool MyClass::registerClass(core::IObjectFactory & _factory)
    // {
    //     core::IObjectDescriptor & desc = _factory.registerClassHelper(MyClass); 
    //     desc.registerMemberHelper(MyClass, m_myMember);
    //     return true;
    // }
    //
    //--------------------------------------------------------------------------------------
    #define registerClassHelper(className)                                          registerClass(#className, [](){ return new className(); })
    #define registerPropertyHelper(className, propertyName, displayName, flags)     registerProperty(#propertyName, (&((className*)(nullptr))->propertyName), displayName, flags)
    #define registerCallbackHelper(className, funcName, displayName, flags)                    registerProperty(#funcName, funcName, displayName, flags)

    class IPropertyDescriptor
    {
    public:

        using Func = bool (__cdecl*)(IObject*);

        enum class Type : u32
        {
            Undefined = 0,
            Bool,
            Float4,
            String,
            Function,
        };

        enum class Flags : u64
        {
            None        = 0x0000000000000000,
            ReadOnly    = 0x0000000000000001,
            Color       = 0x0000000000000002
        };

        virtual const char *                getName                 () const = 0;
        virtual const char *                getDisplayName          () const = 0;
        virtual Type                        getType                 () const = 0;
        virtual Flags                       getFlags                () const = 0;
        virtual uint_ptr                    getOffset               () const = 0;
    };

    class IObjectDescriptor
    {
    public:
        virtual                             ~IObjectDescriptor  () {}

        virtual void                        registerProperty    (const char * _propertyName, bool * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty    (const char * _propertyName, core::float4 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty    (const char * _propertyName, core::string * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty    (const char * _propertyName, IPropertyDescriptor::Func _funcPtr, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;

        virtual const char *                getClassName        () const = 0;
        virtual uint                        getPropertyCount    () const = 0;
        virtual const IPropertyDescriptor * getProperty         (uint _index) const = 0;
    };

    class IObjectFactory
    {
    public:
        virtual ~IObjectFactory() {}

        using CreateFunc = std::function<Object*()>;

        virtual IObjectDescriptor &         registerClass       (const char * _className, CreateFunc _createFunc) = 0;
        virtual const IObjectDescriptor *   getClassDescriptor  (const char * _className) const = 0;
        virtual bool                        isRegisteredClass   (const char * _className) const = 0;

        virtual bool                        serializeFromString (IObject * _object, const string & _in) const = 0;
        virtual bool                        serializeToString   (string & _out, const IObject * _object) const = 0;
    };
}