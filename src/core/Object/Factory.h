#pragma once

#include "core/IFactory.h"
#include "ClassDesc.h"

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

        IClassDesc *                                registerClass                   (const char * _interfaceName, const char * _className, const char * _classDisplayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::Func _createFunc) final;
        IClassDesc *                                registerSingletonClass          (const char * _interfaceName, const char * _className, const char * _classDisplayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::SingletonFunc _createFunc) final;
        const IClassDesc *                          getClassDescriptor              (const char * _className) const final;
        const vector<IClassDesc *>                  getClassDescriptors             (IClassDesc::Flags _required = (IClassDesc::Flags)-1, IClassDesc::Flags _excluded = (IClassDesc::Flags)0) const final;
        bool                                        isRegisteredClass               (const char * _className) const final;
        IObject *                                   getSingleton                    (const char * _className) const final;
        IObject *                                   createObject                    (const char * _className, const string & _name, IObject * _parent = nullptr) const final;

        bool                                        loadFromXML                     (IObject * _object, const string & _XMLfilename) const final;
        bool                                        saveToXML                       (const IObject * _object, const string & _xmlFile) const final;

        bool                                        serializeFromXML                (IObject * _object, XMLDoc & _xmlDoc) const final;
        bool                                        serializeToXML                  (const IObject * _object, XMLDoc & _xmlDoc, XMLElem * _parent = nullptr) const final;

        bool                                        SaveProperties                  (core::IObject * _object) final override;
        bool                                        RestoreProperties               (core::IObject * _object) final override;
        bool                                        CopyProperties                  (const core::IObject * _srcObj, core::IObject * _dstObj) final override;
        IObject *                                   Instanciate                     (const core::IObject * _object, IObject * _parent) final override;

    protected:
        bool                                        serializeFromXML                (IObject * _object, const XMLElem * _xmlElem) const;

        template <typename T> void                  serializeIntegerPropertyFromXML (IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const;
        template <typename T> void                  serializeIntegerPropertyToXML   (const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem) const;

        template <typename T> void                  serializeEnumPropertyFromXML    (IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const;
        template <typename T> void                  serializeEnumPropertyToXML      (const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem) const;

        template <typename T> void                  serializeEnumFlagsPropertyFromXML(IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const;
        template <typename T> void                  serializeEnumFlagsPropertyToXML (const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem) const;

        bool                                        serializeToMemory               (const IObject * _object, io::Buffer & _buffer);
        bool                                        serializeFromMemory             (IObject * _object, io::Buffer & _buffer);

        void                                        ReleaseAsync                    (core::IObject * _object);
        void                                        FlushReleaseAsync               ();

         template <typename T> static string        getEnumFlagsPropertyAsString    (T _value, const IProperty * _prop);

    private:
        core::vector<ClassDesc>                     m_classes;
        core::dictionary<IProperty::Type>           m_oldTypeNames;
        mutex                                       m_objectsToReleaseMutex;
        vector<IObject*>                            m_objectsToRelease[2];
        u8                                          m_objectsToReleaseTableIndex = 0;
        core::unordered_map<IObject *, io::Buffer*> m_initValues;
    };    
}