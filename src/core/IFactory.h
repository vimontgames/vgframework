#pragma once

#include "IClassDesc.h"
#include "XML/XML.h"
#include "core/Object/EnumHelper.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Abstract factory to create Objects
    //--------------------------------------------------------------------------------------
    #define propertyOffset(typeName, className, propertyName)													            (typeName*)&((className*)(nullptr))->propertyName
    
    #define registerClassHelper_NoCTor(className, displayName, flags)											            registerClass(#className, displayName, flags, sizeof(className), [](const vg::core::string & _name, vg::core::IObject * _parent) { return nullptr; })
    #define registerClassHelper(className, displayName, flags)                                                              registerClass(#className, displayName, flags, sizeof(className), [](const vg::core::string & _name, vg::core::IObject * _parent) { auto newObj = new className(_name, _parent); VG_ASSERT(nullptr != dynamic_cast<vg::core::IObject*>(newObj)); return dynamic_cast<vg::core::IObject*>(newObj); }) // 'dynamic_cast' should not be necessary but the cast is present to workaround weird Lambda to std::function conversion compilation issue  
    #define registerClassSingletonHelper(className, displayName, flags)											            registerSingletonClass(#className, displayName, flags | vg::core::IClassDesc::Flags::Singleton, sizeof(className), [](){ return className::get(); } )
    #define registerPlugin(className, displayName)											                                registerSingletonClass(#className, displayName, vg::core::IClassDesc::Flags::Singleton | vg::core::IClassDesc::Flags::Plugin, sizeof(className), [](){ return className::get(); } )

    #define registerPropertyHelperEx(className, propertyName, displayName, flags)									        registerProperty(#className, #propertyName, (&((className*)(nullptr))->propertyName), displayName, flags)
    #define registerPropertyHelper(className, propertyName, displayName)									                registerPropertyHelperEx(className, propertyName, displayName, vg::core::IProperty::Flags::None)
    #define registerPropertyObjectRefHelper(className, propertyName, displayName, flags)                                    registerProperty(#className, #propertyName, (core::IObject**)offsetof(className, propertyName), displayName, flags);
    #define registerPropertyObjectVectorHelper(className, propertyName, elementType, displayName, flags)                    registerProperty(#className, #propertyName, sizeof(elementType), &((className*)nullptr)->propertyName, displayName, flags);
    #define registerPropertyObjectRefVectorHelper(className, propertyName, displayName, flags)                              registerProperty(#className, #propertyName, (core::vector<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
    #define registerPropertyObjectRefDictionaryHelper(className, propertyName, displayName, flags)                          registerProperty(#className, #propertyName, (core::dictionary<core::IObject*>*)&((className*)nullptr)->propertyName, displayName, flags);
    #define registerCallbackHelper(className, funcName, displayName, flags)										            registerProperty(#className, #funcName, funcName, displayName, flags)
	
    //#define registerPropertyEnumHelper(className, enumClassName, propertyName, displayName, eCount, eNames, eVals, flags) registerEnum(#className, #propertyName, (std::underlying_type_t<enumClassName>*)(&((className*)(nullptr))->propertyName), displayName, eCount, eNames, eVals, flags);
    #define registerPropertyEnumWithFlags(className, enumClassName, propertyName, displayName, flags)                       registerEnum(#className, #propertyName, (std::underlying_type_t<enumClassName>*)(&((className*)(nullptr))->propertyName), displayName, vg::core::EnumHelper<enumClassName>::getStaticCount(), vg::core::EnumHelper<enumClassName>::getStaticNames().c_str(), vg::core::EnumHelper<enumClassName>::getStaticValues().data(), flags);
    #define registerPropertyEnum(className, enumClassName, propertyName, displayName)                                       registerPropertyEnumWithFlags(className, enumClassName, propertyName, displayName, vg::core::IProperty::Flags::None)
    #define registerPropertyEnumBitfield(className, enumClassName, propertyName, displayName)                               registerPropertyEnumWithFlags(className, enumClassName, propertyName, displayName, vg::core::IProperty::Flags::Bitfield)

    #define setPropertyRangeHelper(className, propertyName, range)												            getPropertyByName(#propertyName)->setRange(range);

    class IObject;
    class IClassDesc;
    class IResource;

    class IFactory
    {
    public:
        virtual ~IFactory() {}

        virtual IClassDesc *                registerClass           (const char * _className, const char * _displayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::Func _createFunc) = 0;
        virtual IClassDesc *                registerSingletonClass  (const char * _className, const char * _displayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::SingletonFunc _createFunc) = 0;
        virtual const IClassDesc *          getClassDescriptor      (const char * _className) const = 0;
        virtual const vector<IClassDesc *>  getClassDescriptors     (IClassDesc::Flags _required = (IClassDesc::Flags)-1, IClassDesc::Flags _excluded = (IClassDesc::Flags)0) const = 0;
        virtual bool                        isRegisteredClass       (const char * _className) const = 0;
        virtual IObject *                   getSingleton            (const char * _className) const = 0;
        virtual IObject *                   createObject            (const char * _className, const string & _name = "", IObject * _parent = nullptr) const = 0;

        virtual bool                        loadFromXML             (IObject * _object, const string & _XMLfilename) const = 0;
        virtual bool                        saveToXML               (const IObject * _object, const string & _xmlFile) const  = 0;

        virtual bool                        serializeFromXML        (IObject * _object, XMLDoc & _xmlDoc) const = 0;
        virtual bool                        serializeToXML          (const IObject * _object, XMLDoc & _xmlDoc, XMLElem * _parent = nullptr) const = 0;

        virtual void                        ReleaseAsync            (core::IObject * _object) = 0;
        virtual void                        FlushReleaseAsync       () = 0;
    };

    #define CreateFactoryObject(type, name, parent) Kernel::getFactory()->createObject(#type, name, parent)    
}