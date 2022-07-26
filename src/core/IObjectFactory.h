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
    #define propertyOffset(typeName, className, propertyName)													            (typeName*)&((className*)(nullptr))->propertyName

    #define registerClassHelper_NoCTor(className, displayName, flags)											            registerClass(#className, displayName, flags, [](const vg::core::string & _name, vg::core::IObject * _parent) { return nullptr; })
    #define registerClassHelper(className, displayName, flags)													            registerClass(#className, displayName, flags, [](const vg::core::string & _name, vg::core::IObject * _parent) { return new className(_name, _parent); })
    #define registerClassSingletonHelper(className, displayName, flags)											            registerSingletonClass(#className, displayName, flags | vg::core::IObjectDescriptor::Flags::Singleton, [](){ return className::get(); } )

    #define registerPropertyHelper(className, propertyName, displayName, flags)									            registerProperty(#propertyName, (&((className*)(nullptr))->propertyName), displayName, flags)
    #define registerPropertyObjectPointerHelper(className, propertyName, displayName, flags)                                registerProperty(#propertyName, (core::IObject**)offsetof(className, propertyName), displayName, flags);
    #define registerPropertyObjectVectorHelper(className, propertyName, elementType, displayName, flags)                    registerProperty(#propertyName, sizeof(elementType), &((className*)nullptr)->propertyName, displayName, flags);
    #define registerPropertyObjectPointerVectorHelper(className, propertyName, displayName, flags)                          registerProperty(#propertyName, (core::vector<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
    #define registerPropertyObjectPointerDictionaryHelper(className, propertyName, displayName, flags)                      registerProperty(#propertyName, (core::dictionary<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
    #define registerCallbackHelper(className, funcName, displayName, flags)										            registerProperty(#funcName, funcName, displayName, flags)
	#define registerPropertyEnumHelper(className, enumClassName, propertyName, enumValue, displayName, enumValues, flags)	registerEnum(#propertyName, (std::underlying_type_t<enumClassName>*)(&((className*)(nullptr))->propertyName), enumValue, displayName, enumValues, flags);

    #define setPropertyRangeHelper(className, propertyName, range)												            getPropertyByName(#propertyName)->setRange(range);

    class IObject;
    class IResource;

    class IPropertyDescriptor
    {
    public:

        using Func = bool (__cdecl*)(IObject*);

        enum class Type : u32
        {
            Undefined = 0,
            Bool,
            Uint16,
            Uint32,
            Float,
            Float4,
            Matrix44,
            String,
            ObjectPointer,
            Resource,
            Function,
			Enum,
            ObjectVector,
            ObjectPointerVector,
            ObjectPointerDictionary
        };

        enum class Flags : u64
        {
            None        = 0x0000000000000000,
            ReadOnly    = 0x0000000000000001,   // Cannot edit from GUI
            Color       = 0x0000000000000002,   // Type represents a color
            IsFolder    = 0x0000000000000004,   // String is a folder
            HasRange    = 0x0000000000000008,   // Property has [min..max] range
            SameLine    = 0x0000000000000010,   // Do no end line after this property
			Radio		= 0x0000000000000020,   // Part of a radio button group
            Hidden      = 0x0000000000000040    // Hide property
        };

        virtual void                        setRange                (float2 _range) = 0;

        virtual const char *                getName                 () const = 0;
        virtual const char *                getDisplayName          () const = 0;
        virtual const char *                getDescription          () const = 0;
        virtual Type                        getType                 () const = 0;
        virtual Flags                       getFlags                () const = 0;
        virtual uint_ptr                    getOffset               () const = 0;
		virtual core::u32					getValue				() const = 0;
        virtual float2                      getRange                () const = 0;
    };

    class IObjectDescriptor
    {
    public:
        using Func          = std::function<IObject*(const string &, IObject *)>;
        using SingletonFunc = std::function<IObject*()>;

        enum class Flags : u64
        {
            None        = 0x0000000000000000,
            Singleton   = 0x0000000000000001,
            Entity      = 0x0000000000000002,
            Component   = 0x0000000000000004,
            Model       = 0x0000000000000008,
            Instance    = 0x0000000000000010,
            Resource    = 0x0000000000000020,
            SceneNode   = 0x0000000000000040
        };
        virtual                             ~IObjectDescriptor  () {}

        virtual bool                        isRegisteredProperty    (const char * _propertyName) = 0;

        virtual void                        registerProperty        (const char * _propertyName, bool * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _propertyName, core::u32 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _propertyName, core::u16 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _propertyName, float * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _propertyName, float4 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _propertyName, float4x4 * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _propertyName, string * _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _propertyName, IResource ** _offset, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _propertyName, IObject ** _offset, const char * _displayName = nullptr, IPropertyDescriptor::Flags _flags = IPropertyDescriptor::Flags::None) = 0;
        virtual void                        registerProperty        (const char * _propertyName, IPropertyDescriptor::Func _funcPtr, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerProperty        (const char * _propertyName, core::u32 _sizeOf, void * _offset, const char * _displayName = nullptr, IPropertyDescriptor::Flags _flags = IPropertyDescriptor::Flags::None) = 0;
        virtual void                        registerProperty        (const char * _propertyName, vector<IObject*>* _offset, const char * _displayName = nullptr, IPropertyDescriptor::Flags _flags = IPropertyDescriptor::Flags::None) = 0;
        virtual void                        registerProperty        (const char * _propertyName, core::dictionary<core::IObject*>* _offset, const char * _displayName = nullptr, IPropertyDescriptor::Flags _flags = IPropertyDescriptor::Flags::None) = 0;
		virtual void                        registerProperty		(const char * _propertyName, core::u32 * _offset, core::u32 _value, const char * _displayName, IPropertyDescriptor::Flags _flags) = 0;
        virtual void                        registerEnum            (const char * _propertyName, core::u32 * _offset, core::u32 _value, const char * _displayName, const char * _values, IPropertyDescriptor::Flags _flags) = 0;

        virtual const char *                getClassName            () const = 0;
        virtual const char *                getClassDisplayName     () const = 0;
        virtual uint                        getPropertyCount        () const = 0;
        virtual const IPropertyDescriptor * getPropertyByIndex      (uint _index) const = 0;
        virtual IPropertyDescriptor *       getPropertyByName       (const char * _propertyName) const = 0;
        virtual Func                        getCreateFunc           () const = 0;
        virtual SingletonFunc               getSingletonFunc        () const = 0;
        virtual IObjectDescriptor::Flags    getFlags                () const = 0;
        virtual u32                         getNextIndex            () const = 0;
    };

    class IObjectFactory
    {
    public:
        virtual ~IObjectFactory() {}

        virtual IObjectDescriptor *         registerClass           (const char * _className, const char * _displayName, IObjectDescriptor::Flags _flags, IObjectDescriptor::Func _createFunc) = 0;
        virtual IObjectDescriptor *         registerSingletonClass  (const char * _className, const char * _displayName, IObjectDescriptor::Flags _flags, IObjectDescriptor::SingletonFunc _createFunc) = 0;
        virtual const IObjectDescriptor *   getClassDescriptor      (const char * _className) const = 0;
        virtual bool                        isRegisteredClass       (const char * _className) const = 0;
        virtual IObject *                   getSingleton            (const char * _className) const = 0;
        virtual IObject *                   createObject            (const char * _className, const string & _name = "", IObject * _parent = nullptr) const = 0;

        virtual bool                        serializeFromString     (IObject * _object, const string & _in) const = 0;
        virtual bool                        serializeToString       (string & _out, const IObject * _object) const = 0;
    };

    #define CreateFactoryObject(type, name, parent) Kernel::getObjectFactory()->createObject(#type, name, parent)    
}