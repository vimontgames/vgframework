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
        const IClassDesc *                          GetClassDescriptorFromCRC       (ClassCRC _classCRC, bool _mustExist = true) const final;
        const vector<IClassDesc *>                  GetClassDescriptors             (ClassDescFlags _required = (ClassDescFlags)-1, ClassDescFlags _excluded = (ClassDescFlags)0) const final;
        bool                                        IsRegisteredClass               (const char * _className) const final;
        IObject *                                   GetSingleton                    (const char * _className) const final;
        IObject *                                   CreateObject                    (const char * _className, const string & _name, IObject * _parent = nullptr) const final;

        bool                                        LoadFromXML                     (IObject * _object, const string & _xmlFile) const final;
        bool                                        SaveToXML                       (IObject * _object, const string & _xmlFile) const final;

        bool                                        SerializeFromXML                (IObject * _object, XMLDoc & _xmlDoc, const string & _xmlFile) const final;
        bool                                        SerializeToXML                  (IObject * _object, XMLDoc & _xmlDoc, XMLElem * _parent = nullptr) const final;

        bool                                        SaveProperties                  (IObject * _object, BufferType _bufferType) final override;
        bool                                        RestoreProperties               (IObject * _object, BufferType _bufferType) final override;
        bool                                        CopyProperties                  (const IObject * _srcObj, IObject * _dstObj, CopyPropertyFlags _copyPropertyFlags = (CopyPropertyFlags)0x0) final override;
        bool                                        CanCopyProperty                 (const IClassDesc * _srcClassDesc, const IProperty * _srcProp, const IClassDesc * _dstClassDesc, const IProperty * _dstProp) const final override;
        bool                                        CopyProperty                    (const IProperty * _srcProp, const IObject * _srcObj, const IProperty * _dstProp, IObject * _dstObj, CopyPropertyFlags _copyPropertyFlags = (CopyPropertyFlags)0x0) final override;
        IObject *                                   Instanciate                     (const IObject * _object, IObject * _parent, CopyPropertyFlags _copyPropertyFlags = (CopyPropertyFlags)0x0) final override;

        bool                                        IsA                             (ClassCRC _CRC, ClassCRC _targetCRC) const final override;

        UID                                         RegisterUID                     (IObject * _object) final override;
        void                                        ReleaseUID                      (IObject * _object, UID & _uid) final override;
        const UIDObjectHash &                       GetUIDObjects                   () const final override;
        IObject *                                   FindByUID                       (UID _uid) const final override;

        bool                                        serializeObjectToMemory         (const IObject * _object, io::Buffer & _buffer, BufferType _bufferType);
        void                                        serializePropertyToMemory       (const IObject * _object, const IProperty * _prop, io::Buffer & _buffer, BufferType _bufferType);

        bool                                        serializeObjectFromMemory       (IObject * _object, io::Buffer & _buffer, BufferType _bufferType);
        void                                        serializePropertyFromMemory     (IObject * _object, const IProperty * _prop, io::Buffer & _buffer, BufferType _bufferType);

    protected:
        bool                                        SerializeFromXML                (IObject * _object, const XMLElem * _xmlElem, const string & _xmlFile) const;

        template <typename T> void                  serializeIntegerPropertyFromXML (IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem, uint _index = 0) const;
        template <typename T> void                  serializeIntegerPropertyToXML   (const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem, uint _index = 0) const;

        template <typename T> void                  serializeEnumPropertyFromXML    (IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const;
        template <typename T> void                  serializeEnumPropertyToXML      (const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem) const;

        template <typename T> void                  serializeEnumFlagsPropertyFromXML(IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const;
        template <typename T> void                  serializeEnumFlagsPropertyToXML (const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem) const;

        void                                        ReleaseAsync                    (IObject * _object) final override;
        void                                        FlushReleaseAsync               () final override;

        template <typename T> static string         getEnumFlagsPropertyAsString    (T _value, const IProperty * _prop);

         UID                                        getNewUID                       (IObject * _object);

         const char *                               fixDeprecatedClassName          (const char * _className) const;

    private:
        // We need address of ClassDesc to stay constant during program execution, so we use vector + map for indirection to speed up lookups when needed
        vector<ClassDesc>                           m_classes;
        core::unordered_map<ClassCRC, core::uint>   m_classCRCToIndex;
        dictionary<PropertyType>                    m_oldTypeNames;
        dictionary<string>                          m_oldPropertyNames;
        dictionary<const char *>                    m_oldClassNames;
        Mutex                                       m_objectsToReleaseMutex = Mutex("Mutex - FactoryRelease");
        vector<IObject*>                            m_objectsToRelease[2];
        u8                                          m_objectsToReleaseTableIndex = 0;
        unordered_map<UID, io::Buffer *>            m_buffers[enumCount<BufferType>()];
        UIDObjectHash                               m_uidObjectHash;
        mutable Mutex                               m_uidObjectHashMutex = Mutex("Mutex - FactoryObjectHash");
    };    
}