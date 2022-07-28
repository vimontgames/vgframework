#pragma once

#include "IClassDesc.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Abstract factory to create Objects
    //--------------------------------------------------------------------------------------
    #define propertyOffset(typeName, className, propertyName)													            (typeName*)&((className*)(nullptr))->propertyName
    
    #define registerClassHelper_NoCTor(className, displayName, flags)											            registerClass(#className, displayName, flags, [](const vg::core::string & _name, vg::core::IObject * _parent) { return nullptr; })
    #define registerClassHelper(className, displayName, flags)													            registerClass(#className, displayName, flags, [](const vg::core::string & _name, vg::core::IObject * _parent) { return new className(_name, _parent); })
    #define registerClassSingletonHelper(className, displayName, flags)											            registerSingletonClass(#className, displayName, flags | vg::core::IClassDesc::Flags::Singleton, [](){ return className::get(); } )
    
    #define registerPropertyHelper(className, propertyName, displayName, flags)									            registerProperty(#propertyName, (&((className*)(nullptr))->propertyName), displayName, flags)
    #define registerPropertyObjectPointerHelper(className, propertyName, displayName, flags)                                registerProperty(#propertyName, (core::IObject**)offsetof(className, propertyName), displayName, flags);
    #define registerPropertyObjectVectorHelper(className, propertyName, elementType, displayName, flags)                    registerProperty(#propertyName, sizeof(elementType), &((className*)nullptr)->propertyName, displayName, flags);
    #define registerPropertyObjectPointerVectorHelper(className, propertyName, displayName, flags)                          registerProperty(#propertyName, (core::vector<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
    #define registerPropertyObjectPointerDictionaryHelper(className, propertyName, displayName, flags)                      registerProperty(#propertyName, (core::dictionary<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
    #define registerCallbackHelper(className, funcName, displayName, flags)										            registerProperty(#funcName, funcName, displayName, flags)
	#define registerPropertyEnumHelper(className, enumClassName, propertyName, enumValue, displayName, enumValues, flags)	registerEnum(#propertyName, (std::underlying_type_t<enumClassName>*)(&((className*)(nullptr))->propertyName), enumValue, displayName, enumValues, flags);
    
    #define setPropertyRangeHelper(className, propertyName, range)												            getPropertyByName(#propertyName)->setRange(range);

    class IObject;
    class IClassDesc;
    class IResource;

    class IFactory
    {
    public:
        virtual ~IFactory() {}

        virtual IClassDesc *         registerClass           (const char * _className, const char * _displayName, IClassDesc::Flags _flags, IClassDesc::Func _createFunc) = 0;
        virtual IClassDesc *         registerSingletonClass  (const char * _className, const char * _displayName, IClassDesc::Flags _flags, IClassDesc::SingletonFunc _createFunc) = 0;
        virtual const IClassDesc *   getClassDescriptor      (const char * _className) const = 0;
        virtual bool                        isRegisteredClass       (const char * _className) const = 0;
        virtual IObject *                   getSingleton            (const char * _className) const = 0;
        virtual IObject *                   createObject            (const char * _className, const string & _name = "", IObject * _parent = nullptr) const = 0;

        virtual bool                        serializeFromString     (IObject * _object, const string & _in) const = 0;
        virtual bool                        serializeToString       (string & _out, const IObject * _object) const = 0;
    };

    #define CreateFactoryObject(type, name, parent) Kernel::getFactory()->createObject(#type, name, parent)    
}