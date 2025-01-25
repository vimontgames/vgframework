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
    class IGameObject;
    class IComponent;

    using UID = core::u32;

    vg_enum_class(BufferType, core::u8,
        InitValue = 0,
        UndoRedo
    );

    vg_enum_class(CopyPropertyFlags, core::u32,
        NoChildren = 0x00000001
    );

    class IFactory
    {
    public:
        virtual ~IFactory() {}

        virtual IClassDesc *                RegisterObjectClass         (const char * _parentClassName, const char * _className, const char * _classDisplayName, ClassDescFlags _flags, u32 sizeOf, IClassDesc::Func _createFunc) = 0;
        virtual IClassDesc *                RegisterSingletonClass      (const char * _parentClassName, const char * _className, const char * _classDisplayName, ClassDescFlags _flags, u32 sizeOf, IClassDesc::SingletonFunc _createFunc) = 0;
        virtual const IClassDesc *          GetClassDescriptor          (const char * _className, bool _mustExist = true) const = 0;
        virtual const vector<IClassDesc *>  GetClassDescriptors         (ClassDescFlags _required = (ClassDescFlags)-1, ClassDescFlags _excluded = (ClassDescFlags)0) const = 0;
        virtual bool                        IsRegisteredClass           (const char * _className) const = 0;
        virtual IObject *                   GetSingleton                (const char * _className) const = 0;
        virtual IObject *                   CreateObject                (const char * _className, const string & _name = "", IObject * _parent = nullptr) const = 0;

        virtual bool                        LoadFromXML                 (IObject * _object, const string & _XMLfilename) const = 0;
        virtual bool                        SaveToXML                   (IObject * _object, const string & _xmlFile) const  = 0;

        virtual bool                        SerializeFromXML            (IObject * _object, XMLDoc & _xmlDoc) const = 0;
        virtual bool                        SerializeToXML              (IObject * _object, XMLDoc & _xmlDoc, XMLElem * _parent = nullptr) const = 0;

        virtual void                        ReleaseAsync                (core::IObject * _object) = 0;
        virtual void                        FlushReleaseAsync           () = 0;

        virtual bool                        SaveProperties              (core::IObject * _object, BufferType _bufferType) = 0; 
        virtual bool                        RestoreProperties           (core::IObject * _object, BufferType _bufferType) = 0;
        virtual bool                        CopyProperties              (const core::IObject * _srcObj, core::IObject * _dstObj, CopyPropertyFlags _copyPropertyFlags = (CopyPropertyFlags)0x0) = 0;
        virtual bool                        CanCopyProperty             (const core::IProperty * _srcProp, const core::IProperty * _dstProp) const = 0;
        virtual bool                        CopyProperty                (const core::IProperty * _srcProp, const core::IObject * _srcObj, const core::IProperty * _dstProp, core::IObject * _dstObj, CopyPropertyFlags _copyPropertyFlags = (CopyPropertyFlags)0x0) = 0;
        virtual IObject *                   Instanciate                 (const core::IObject * _object, IObject * _parent = nullptr, CopyPropertyFlags _copyPropertyFlags = (CopyPropertyFlags)0x0) = 0;

        virtual bool                        IsA                         (const char * _class, const char * _other) const = 0;

        using UIDObjectHash = core::unordered_map<UID, IObject *>;

        virtual UID                         RegisterUID                 (IObject * _object) = 0;
        virtual void                        ReleaseUID                  (IObject * _object, UID & _uid) = 0;
        virtual const UIDObjectHash &       GetUIDObjects               () const = 0;
        virtual IObject *                   FindByUID                   (UID _uid) const = 0;
    };  
}

//--------------------------------------------------------------------------------------
// Create any Object from its className
//--------------------------------------------------------------------------------------
#define CreateFactoryObject(type, name, parent)                         Kernel::getFactory()->CreateObject(#type, name, parent)  

//--------------------------------------------------------------------------------------
// Register object class macros
//--------------------------------------------------------------------------------------
#define registerClassHelper(className, displayName, flags)              RegisterObjectClass(className::super::getStaticClassName(), #className, displayName, flags, sizeof(className), [](const vg::core::string & _name, vg::core::IObject * _parent) { auto newObj = new className(_name, _parent); VG_ASSERT(nullptr != dynamic_cast<vg::core::IObject*>(newObj)); return dynamic_cast<vg::core::IObject*>(newObj); }) // 'dynamic_cast' should not be necessary but the cast is present to workaround weird Lambda to std::function conversion compilation issue  
#define registerInterfaceHelper(className, displayName, flags)          RegisterObjectClass(className::super::getStaticClassName(), #className, displayName, flags, sizeof(className), [](const vg::core::string & _name, vg::core::IObject * _parent) { return nullptr; }) // 'dynamic_cast' should not be necessary but the cast is present to workaround weird Lambda to std::function conversion compilation issue  
#define registerClassSingletonHelper(className, displayName, flags)     RegisterSingletonClass(className::super::getStaticClassName(), #className, displayName, flags | vg::core::ClassDescFlags::Singleton, sizeof(className), [](){ return className::get(); } )
#define registerPlugin(className, displayName)                          RegisterSingletonClass(className::super::getStaticClassName(), #className, displayName, vg::core::ClassDescFlags::Singleton | vg::core::ClassDescFlags::Plugin, sizeof(className), [](){ return className::get(); } )
