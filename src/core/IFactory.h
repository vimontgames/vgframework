#pragma once

#include "IClassDesc.h"
#include "core/Object/RegisterPropertiesMacros.h"
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

        virtual IClassDesc *                registerClass               (const char * _className, const char * _displayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::Func _createFunc) = 0;
        virtual IClassDesc *                registerSingletonClass      (const char * _className, const char * _displayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::SingletonFunc _createFunc) = 0;
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

    #define CreateFactoryObject(type, name, parent) Kernel::getFactory()->createObject(#type, name, parent)    
}