#pragma once

#include "core/IFactory.h"
#include "ClassDesc.h"

namespace vg::core
{
    class Object;

    class Factory : public IFactory
    {
    public:

        IClassDesc *                registerClass           (const char * _className, const char * _displayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::Func _createFunc) final;
        IClassDesc *                registerSingletonClass  (const char * _className, const char * _displayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::SingletonFunc _createFunc) final;
        const IClassDesc *          getClassDescriptor      (const char * _className) const final;
        bool                        isRegisteredClass       (const char * _className) const final;
        IObject *                   getSingleton            (const char * _className) const final;
        IObject *                   createObject            (const char * _className, const string & _name, IObject * _parent = nullptr) const final;

        IObject *                   createFromXML           (const string & _XMLfilename) const final;
        bool                        loadFromXML             (IObject * _object, const string & _XMLfilename) const final;
        bool                        saveToXML               (const IObject * _object, const string & _xmlFile) const final;

        bool                        serializeFromString     (IObject * _object, const string & _in) const final;
        bool                        serializeToString       (string & _out, const IObject * _object) const final;

        bool                        serializeFromXML        (IObject * _object, XMLDoc & _xmlDoc) const final;
        bool                        serializeToXML          (const IObject * _object, XMLDoc & _xmlDoc, XMLElem * _parent = nullptr) const final;

    protected:
        bool                        serializeFromXML        (IObject* _object, const XMLElem * _xmlObject) const;

    private:
        core::vector<ClassDesc>     m_classes;
    };    
}