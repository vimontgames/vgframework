#pragma once

#include "core/IFactory.h"
#include "ClassDesc.h"
#include "core/Scheduler/Mutex.h"

namespace vg::core
{
    namespace io
    {
        class Buffer;
    }

    class Object;

    class Factory : public IFactory
    {
    public:
                                                    Factory                         ();
                                                    ~Factory                        ();

        IClassDesc *                                RegisterObjectClass             (const char * _parentClassName, const char * _className, const char * _classDisplayName, ClassDescFlags _flags, u32 sizeOf, IClassDesc::Func _createFunc) final;
        IClassDesc *                                RegisterSingletonClass          (const char * _parentClassName, const char * _className, const char * _classDisplayName, ClassDescFlags _flags, u32 sizeOf, IClassDesc::SingletonFunc _createFunc) final;
        const IClassDesc *                          GetClassDescriptor              (const char * _className, bool _mustExist = true) const final;
        const vector<IClassDesc *>                  GetClassDescriptors             (ClassDescFlags _required = (ClassDescFlags)-1, ClassDescFlags _excluded = (ClassDescFlags)0) const final;
        bool                                        IsRegisteredClass               (const char * _className) const final;
        IObject *                                   GetSingleton                    (const char * _className) const final;
        IObject *                                   CreateObject                    (const char * _className, const string & _name, IObject * _parent = nullptr) const final;

        bool                                        LoadFromXML                     (IObject * _object, const string & _xmlFile) const final;
        bool                                        SaveToXML                       (IObject * _object, const string & _xmlFile) const final;

        bool                                        SerializeFromXML                (IObject * _object, XMLDoc & _xmlDoc) const final;
        bool                                        SerializeToXML                  (IObject * _object, XMLDoc & _xmlDoc, XMLElem * _parent = nullptr) const final;

        bool                                        SaveProperties                  (core::IObject * _object, BufferType _bufferType) final override;
        bool                                        RestoreProperties               (core::IObject * _object, BufferType _bufferType) final override;
        bool                                        CopyProperties                  (const core::IObject * _srcObj, core::IObject * _dstObj) final override;
        bool                                        CanCopyProperty                 (const core::IProperty * _srcProp, const core::IProperty * _dstProp) const final override;
        bool                                        CopyProperty                    (const core::IProperty * _srcProp, const core::IObject * _srcObj, const core::IProperty * _dstProp, core::IObject * _dstObj) final override;
        IObject *                                   Instanciate                     (const core::IObject * _object, IObject * _parent) final override;

        bool                                        IsA                             (const char * _class, const char * _other) const final override;

        UID                                         RegisterUID                     (IObject * _object) final override;
        void                                        ReleaseUID                      (IObject * _object, UID & _uid) final override;
        const UIDObjectHash &                       GetUIDObjects                   () const final override;
        IObject *                                   FindByUID                       (UID _uid) const final override;

    //protected:
        bool                                        SerializeFromXML                (IObject * _object, const XMLElem * _xmlElem) const;

        template <typename T> void                  serializeIntegerPropertyFromXML (IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem, core::uint _index = 0) const;
        template <typename T> void                  serializeIntegerPropertyToXML   (const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem, core::uint _index = 0) const;

        template <typename T> void                  serializeEnumPropertyFromXML    (IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const;
        template <typename T> void                  serializeEnumPropertyToXML      (const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem) const;

        template <typename T> void                  serializeEnumFlagsPropertyFromXML(IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const;
        template <typename T> void                  serializeEnumFlagsPropertyToXML (const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem) const;

        bool                                        serializeObjectToMemory         (const IObject * _object, io::Buffer & _buffer, BufferType _bufferType);
        void                                        serializePropertyToMemory       (const IObject * _object, const IProperty * _prop, io::Buffer & _buffer, BufferType _bufferType);

        bool                                        serializeObjectFromMemory       (IObject * _object, io::Buffer & _buffer, BufferType _bufferType);
        void                                        serializePropertyFromMemory     (IObject * _object, const IProperty * _prop, io::Buffer & _buffer, BufferType _bufferType);

        void                                        ReleaseAsync                    (core::IObject * _object);
        void                                        FlushReleaseAsync               ();

         template <typename T> static string        getEnumFlagsPropertyAsString    (T _value, const IProperty * _prop);

         UID                                        getNewUID                       (IObject * _object);

         const char *                               fixDeprecatedClassName          (const char * _className) const;

    private:
        vector<ClassDesc>                     m_classes;
        dictionary<PropertyType>              m_oldTypeNames;
        dictionary<string>                    m_oldPropertyNames;
        dictionary<const char *>              m_oldClassNames;
        Mutex                                 m_objectsToReleaseMutex = core::Mutex("Mutex - FactoryRelease");
        vector<IObject*>                      m_objectsToRelease[2];
        u8                                    m_objectsToReleaseTableIndex = 0;
        unordered_map<UID, io::Buffer *>      m_buffers[enumCount<BufferType>()];
        UIDObjectHash                         m_uidObjectHash;
        mutable Mutex                         m_uidObjectHashMutex = core::Mutex("Mutex - FactoryObjectHash");
    };    
}