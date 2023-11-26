#pragma once

#include "IClassDesc.h"
#include "core/Object/EnumHelper.h"
#include "XML/XML.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Abstract factory to create Objects
    //--------------------------------------------------------------------------------------
    
    class IObject;
    class IClassDesc;
    class IResource;

    class IFactory
    {
    public:
        virtual ~IFactory() {}

        virtual IClassDesc *                registerClass               (const char * _interfaceName, const char * _className, const char * _classDisplayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::Func _createFunc) = 0;
        virtual IClassDesc *                registerSingletonClass      (const char * _interfaceName, const char * _className, const char * _classDisplayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::SingletonFunc _createFunc) = 0;
        virtual const IClassDesc *          getClassDescriptor          (const char * _className) const = 0;
        virtual const vector<IClassDesc *>  getClassDescriptors         (IClassDesc::Flags _required = (IClassDesc::Flags)-1, IClassDesc::Flags _excluded = (IClassDesc::Flags)0) const = 0;
        virtual bool                        isRegisteredClass           (const char * _className) const = 0;
        virtual IObject *                   getSingleton                (const char * _className) const = 0;
        virtual IObject *                   createObject                (const char * _className, const string & _name = "", IObject * _parent = nullptr) const = 0;

        virtual bool                        loadFromXML                 (IObject * _object, const string & _XMLfilename) const = 0;
        virtual bool                        saveToXML                   (const IObject * _object, const string & _xmlFile) const  = 0;

        virtual bool                        serializeFromXML            (IObject * _object, XMLDoc & _xmlDoc) const = 0;
        virtual bool                        serializeToXML              (const IObject * _object, XMLDoc & _xmlDoc, XMLElem * _parent = nullptr) const = 0;

        virtual void                        ReleaseAsync                (core::IObject * _object) = 0;
        virtual void                        FlushReleaseAsync           () = 0;
    };  
}

//--------------------------------------------------------------------------------------
// Create any Object from its className
//--------------------------------------------------------------------------------------
#define CreateFactoryObject(type, name, parent)                         Kernel::getFactory()->createObject(#type, name, parent)  

//--------------------------------------------------------------------------------------
// Register object class macros
//--------------------------------------------------------------------------------------
#define registerClassHelper(className, displayName, flags)              registerClass(className::super::getStaticClassName(), #className, displayName, flags, sizeof(className), [](const vg::core::string & _name, vg::core::IObject * _parent) { auto newObj = new className(_name, _parent); VG_ASSERT(nullptr != dynamic_cast<vg::core::IObject*>(newObj)); return dynamic_cast<vg::core::IObject*>(newObj); }) // 'dynamic_cast' should not be necessary but the cast is present to workaround weird Lambda to std::function conversion compilation issue  
#define registerClassSingletonHelper(className, displayName, flags)     registerSingletonClass(className::super::getStaticClassName(), #className, displayName, flags | vg::core::IClassDesc::Flags::Singleton, sizeof(className), [](){ return className::get(); } )
#define registerPlugin(className, displayName)                          registerSingletonClass(className::super::getStaticClassName(), #className, displayName, vg::core::IClassDesc::Flags::Singleton | vg::core::IClassDesc::Flags::Plugin, sizeof(className), [](){ return className::get(); } )
