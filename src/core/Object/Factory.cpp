#include "core/Precomp.h"
#include "Factory.h"
#include "core/Math/Math.h"
#include "core/IResource.h"
#include "core/XML/XML.h"
#include "core/Timer/Timer.h"
#include "core/Kernel.h"
#include "engine/IResourceManager.h"
#include "core/File/File.h"

using namespace tinyxml2;

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Returned pointer shall not be stored but used immediatly ;)
    //--------------------------------------------------------------------------------------
    IClassDesc * Factory::registerClass(const char * _className, const char * _displayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::Func _createFunc)
    {
        // Classes declared in shared static libs could be declared more than once at static init
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            ClassDesc & desc = m_classes[i];
            if (!strcmp(desc.getClassName(), _className))
            {
                VG_INFO("[Factory] Class \"%s\" is already registered", _className);
                return nullptr; 
            }
        }

        VG_INFO("[Factory] Register class \"%s\"", _className);

        ClassDesc classDesc;
        classDesc.name = _className;
        classDesc.displayName = _displayName ? _displayName : _className;
        classDesc.flags = _flags;
        classDesc.sizeOf = sizeOf;
        classDesc.createFunc = _createFunc;

        m_classes.push_back(classDesc);

        return &m_classes.back();
    }

    //--------------------------------------------------------------------------------------
    IClassDesc * Factory::registerSingletonClass(const char * _className, const char * _displayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::SingletonFunc _singletonFunc)
    {
        // Classes declared in shared static libs could be declared more than once at static init
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            ClassDesc & desc = m_classes[i];
            if (!strcmp(desc.getClassName(), _className))
            {
                VG_INFO("[Factory] Singleton Class \"%s\" is already registered", _className);
                return nullptr;
            }
        }

        VG_INFO("[Factory] Register singleton class \"%s\"", _className);

        ClassDesc classDesc;
        classDesc.name = _className;
        classDesc.displayName = _displayName ? _displayName : _className;
        classDesc.flags = _flags;
        classDesc.sizeOf = sizeOf;
        classDesc.createSingletonFunc = _singletonFunc;

        m_classes.push_back(classDesc);

        return &m_classes.back();
    }

    //--------------------------------------------------------------------------------------
    bool Factory::isRegisteredClass(const char * _className) const
    {
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            const auto & classDesc = m_classes[i];
            if (!strcmp(classDesc.name, _className))
                return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    IObject * Factory::getSingleton(const char * _className) const
    {
        const auto * desc = getClassDescriptor(_className);
        if (desc)
        {
            VG_ASSERT(asBool(IClassDesc::Flags::Singleton & desc->getFlags()));
            return desc->getSingletonFunc()();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    IObject * Factory::createObject(const char * _className, const string & _name, IObject * _parent) const
    {
        auto * desc = getClassDescriptor(_className);

        if (desc)
        {
            VG_ASSERT(!asBool(IClassDesc::Flags::Singleton & desc->getFlags()));
            IObject * obj = desc->getCreateFunc()(_name, _parent);

            const auto index = desc->getNextIndex();

            if (_name.length())
                obj->setName(_name);
            else
                obj->setName((string)obj->getClassName() + " #" + to_string(index));

            return obj;
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::loadFromXML(IObject * _object, const string & _XMLfilename) const
    {
        const auto startLoad = Timer::getTick();
        const auto * factory = Kernel::getFactory();
        string relativePath = io::getRelativePath(_XMLfilename);

        XMLDoc xmlDoc;
        if (XMLError::XML_SUCCESS == xmlDoc.LoadFile(relativePath.c_str()))
        {
            XMLNode * xmlRoot = xmlDoc.FirstChild();
            if (xmlRoot != nullptr)
            {
                VG_INFO("[Factory] Load \"%s\"", relativePath.c_str());
                if (factory->serializeFromXML(_object, xmlDoc))
                {
                    VG_INFO("[Factory] \"%s\" loaded in %.2f ms", relativePath.c_str(), Timer::getEnlapsedTime(startLoad, Timer::getTick()));
                    _object->setFile(relativePath.c_str());
                    return true;
                }
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::saveToXML(const IObject * _object, const string & _xmlFile) const
    {
        const auto * factory = Kernel::getFactory();

        XMLDoc xmlDoc;
        XMLNode * xmlRoot = xmlDoc.NewElement("Root");
        xmlDoc.InsertFirstChild(xmlRoot);

        if (factory->serializeToXML(_object, xmlDoc))
            if (XMLError::XML_SUCCESS == xmlDoc.SaveFile(_xmlFile.c_str()))
                return true;

        return false;
    }

    //--------------------------------------------------------------------------------------
    const IClassDesc * Factory::getClassDescriptor(const char * _className) const
    {
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            const auto & classDesc = m_classes[i];
            if (!strcmp(classDesc.name, _className))
                return &classDesc;
        }

        VG_ASSERT(false, "class \"%s\" has no class descriptor", _className);
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    const vector<IClassDesc *> Factory::getClassDescriptors(IClassDesc::Flags _required, IClassDesc::Flags _excluded) const
    {
        vector<IClassDesc *> classes;
        for (uint i = 0; i < m_classes.count(); ++i)
        {
            auto & clas = m_classes[i];
            if (asBool(clas.flags & _required) && !asBool(clas.flags & _excluded))
                classes.push_back((IClassDesc *) &clas);
        }
        return classes;
    }

    //--------------------------------------------------------------------------------------
    template <typename ENUM_TYPE> ENUM_TYPE enumGetValue(const char * _value, ENUM_TYPE _default = (ENUM_TYPE)0)
    {
        for (uint e = 0; e < enumCount<ENUM_TYPE>(); ++e)
        {
            ENUM_TYPE enumVal = (ENUM_TYPE)e;
            const std::string_view & str = magic_enum::enum_name(enumVal);
            if (!strcmp(str.data(), _value))
                return enumVal;
        }
        return _default;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::serializeFromXML(IObject* _object, XMLDoc & _xmlDoc) const
    {
        auto * parent = _xmlDoc.RootElement();
        const XMLElement * xmlObject = parent->FirstChildElement("Object");
        return serializeFromXML(_object, xmlObject);
    }

    //--------------------------------------------------------------------------------------
    bool Factory::serializeFromXML(IObject * _object, const XMLElem * xmlObject) const
    {
        if (_object == nullptr)
        {
            VG_ASSERT(nullptr != _object, "Cannot serialize nullptr");
            return false;
        }

        if (nullptr != xmlObject)
        {
            const XMLAttribute * xmlClassAttr = xmlObject->FindAttribute("class");
            if (nullptr != xmlClassAttr)
            {
                const char * className = xmlClassAttr->Value();
                const auto * classDesc = getClassDescriptor(className);
                if (nullptr != classDesc)
                {
                    const XMLElement * xmlPropElem = xmlObject->FirstChildElement("Property");
                    while (nullptr != xmlPropElem)
                    {
                        const XMLAttribute * xmlName = xmlPropElem->FindAttribute("name");
                        const XMLAttribute * xmlType = xmlPropElem->FindAttribute("type");
                        if (nullptr != xmlName && nullptr != xmlType)
                        {
                            const char * name = xmlName->Value();
                            const char * typeName = xmlType->Value();
                            auto type = enumGetValue<IProperty::Type>(typeName);
                            const auto * prop = classDesc->getPropertyByName(name);
                            if (nullptr != prop)
                            {
                                if (prop->getType() == type)
                                {
                                    const auto offset = prop->getOffset();

                                    switch (type)
                                    {
                                        default:
                                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                                            break;

                                        case IProperty::Type::Float:
                                        {
                                            const XMLAttribute* xmlValue = xmlPropElem->FindAttribute("value");
                                            if (nullptr != xmlValue)
                                            {
                                                float* pFloat = (float*)(uint_ptr(_object) + offset);
                                                *pFloat = xmlValue->FloatValue();
                                            }
                                        }
                                        break;

                                        case IProperty::Type::Float4x4:
                                        {
                                            float* pFloat4x4 = (float*)(uint_ptr(_object) + offset);
                                            const char * values[] = 
                                            { 
                                                "Ix", "Iy", "Iz", "Iw", 
                                                "Jx", "Jy", "Jz", "Jw",
                                                "Kx", "Ky", "Kz", "Kw",
                                                "Tx", "Ty", "Tz", "Tw",
                                            };

                                            for (uint i = 0; i < countof(values); ++i)
                                            {
                                                const XMLAttribute* xmlValue = xmlPropElem->FindAttribute(values[i]);
                                                if (nullptr != xmlValue)
                                                    pFloat4x4[i] = xmlValue->FloatValue();
                                            }
                                        }
                                        break;

                                        case IProperty::Type::ObjectRef:
                                        {
                                            IObject* pObjectRef = nullptr;
                                            const XMLElement* xmlObjectRef = xmlPropElem->FirstChildElement("Object");
                                            if (nullptr != xmlObjectRef)
                                            {
                                                const XMLAttribute* xmlClassAttrRef = xmlObjectRef->FindAttribute("class");
                                                if (nullptr != xmlClassAttrRef)
                                                {
                                                    const char* classNameRef = xmlClassAttrRef->Value();
                                                    const auto* classDescRef = getClassDescriptor(classNameRef);
                                                    if (nullptr != classDescRef)
                                                    {
                                                        pObjectRef = createObject(classNameRef, "", _object);
                                                        if (!serializeFromXML(pObjectRef, xmlObjectRef))
                                                            VG_SAFE_DELETE(pObjectRef);
                                                    }
                                                }
                                            }
                                        
                                            IObject** ppObjectRef = (IObject**)(uint_ptr(_object) + offset);
                                            VG_SAFE_RELEASE((*ppObjectRef));
                                            *ppObjectRef = pObjectRef;
                                        }
                                        break;

                                        case IProperty::Type::ObjectRefVector:
                                        {
                                            const XMLElement* xmlObjectRef = xmlPropElem->FirstChildElement("Object");
                                            if (nullptr != xmlObjectRef)
                                            {
                                                auto * vector = prop->GetPropertyObjectRefVector(_object);
                                                const XMLElement* xmlObjectRef = xmlPropElem->FirstChildElement("Object");
                                        
                                                do
                                                {
                                                    IObject* pObjectRef = nullptr;
                                        
                                                    if (nullptr != xmlObjectRef)
                                                    {
                                                        const XMLAttribute* xmlClassAttrRef = xmlObjectRef->FindAttribute("class");
                                                        if (nullptr != xmlClassAttrRef)
                                                        {
                                                            const char* classNameRef = xmlClassAttrRef->Value();
                                                            const auto* classDescRef = getClassDescriptor(classNameRef);
                                                            if (nullptr != classDescRef)
                                                            {
                                                                pObjectRef = createObject(classNameRef, "", _object);
                                                                if (serializeFromXML(pObjectRef, xmlObjectRef))
                                                                    vector->push_back(pObjectRef);
                                                                else
                                                                    VG_SAFE_DELETE(pObjectRef);
                                                            }
                                                        }
                                                    }
                                        
                                                    xmlObjectRef = xmlObjectRef->NextSiblingElement("Object");
                                        
                                                } while (xmlObjectRef != nullptr);
                                            }
                                        }
                                        break;

                                        case IProperty::Type::Resource:
                                        {
                                            IResource* pResource = (IResource*)(uint_ptr(_object) + offset);
                                            const IClassDesc* classDescRef = nullptr;
                                            const XMLElement* xmlObjectRef = xmlPropElem->FirstChildElement("Object");
                                            if (nullptr != xmlObjectRef)
                                            {
                                                const XMLAttribute* xmlClassAttrRef = xmlObjectRef->FindAttribute("class");
                                                if (nullptr != xmlClassAttrRef)
                                                {
                                                    const char* classNameRef = xmlClassAttrRef->Value();
                                                    classDescRef = getClassDescriptor(classNameRef);
                                                    if (nullptr != classDescRef)
                                                    {
                                                        if (serializeFromXML(pResource, xmlObjectRef))
                                                            pResource->onResourcePathChanged("", pResource->GetResourcePath());
                                                    }
                                                }
                                            }
                                        }
                                        break;

                                        case IProperty::Type::EnumFlagsU8:
                                            serializeEnumFlagsPropertyFromXML<u8>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::EnumFlagsU16:
                                            serializeEnumFlagsPropertyFromXML<u16>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::EnumFlagsU32:
                                            serializeEnumFlagsPropertyFromXML<u32>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::EnumFlagsU64:
                                            serializeEnumFlagsPropertyFromXML<u64>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::String:
                                        {
                                            const XMLAttribute * xmlValue = xmlPropElem->FindAttribute("value");
                                            if (nullptr != xmlValue)
                                            {
                                                string * pString = (string*)(uint_ptr(_object) + offset);
                                                *pString = xmlValue->Value();
                                            }
                                        }
                                        break;

                                        case IProperty::Type::Bool:
                                        {
                                            const XMLAttribute * xmlValue = xmlPropElem->FindAttribute("value");
                                            if (nullptr != xmlValue)
                                            {
                                                bool * pBool = (bool*)(uint_ptr(_object) + offset);
                                                *pBool = xmlValue->BoolValue();
                                            }
                                        }
                                        break;

                                        case IProperty::Type::Float4:
                                        {
                                            float * pFloat4 = (float*)(uint_ptr(_object) + offset);
                                            const char * values[] = { "x", "y", "z", "w" };
                                            for (uint i = 0; i < countof(values); ++i)
                                            {
                                                const XMLAttribute * xmlValue = xmlPropElem->FindAttribute(values[i]);
                                                if (nullptr != xmlValue)
                                                    pFloat4[i] = xmlValue->FloatValue();
                                            }
                                        }
                                        break;

                                        case IProperty::Type::EnumU8:
                                            serializeEnumPropertyFromXML<u8>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::EnumU16:
                                            serializeEnumPropertyFromXML<u16>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::EnumU32:
                                            serializeEnumPropertyFromXML<u32>(_object, prop, xmlPropElem);
                                        break;

                                        case IProperty::Type::Uint8:
                                            serializeIntegerPropertyFromXML<u8>(_object, prop, xmlPropElem);
                                        break;

                                        case IProperty::Type::Uint16:
                                            serializeIntegerPropertyFromXML<u16>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::Uint32:
                                            serializeIntegerPropertyFromXML<u32>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::Uint64:
                                            serializeIntegerPropertyFromXML<u64>(_object, prop, xmlPropElem);
                                            break;
                                    }
                                }
                                else
                                {
                                    VG_DEBUGPRINT("[Factory] Serialized Object type \"%s\" for Property \"%s\" from class \"%s\" does not match type \"%s\" declared in ClassDesc\n", typeName, name, className, asString(prop->getType()));
                                }
                            }
                        }

                        xmlPropElem = xmlPropElem->NextSiblingElement("Property");
                    }
                }
            }
        }
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::serializeToXML(const IObject * _object, XMLDoc & _xmlDoc, XMLElem * _parent) const
    {
        auto * parent = nullptr != _parent ? _parent : _xmlDoc.RootElement();

        const char * className = _object->getClassName();

        XMLElement * xmlElement = _xmlDoc.NewElement("Object");
        xmlElement->SetAttribute("class", className);

        const auto * classDesc = getClassDescriptor(className);

        for (uint p = 0; p < classDesc->getPropertyCount(); ++p)
        {
            const auto & prop = classDesc->getPropertyByIndex(p);

            const auto name = prop->getName();
            const auto type = prop->getType();
            const auto offset = prop->getOffset();
            const auto flags = prop->getFlags();

            if (asBool(IProperty::Flags::NotSaved & flags))
                continue;

            XMLElement * xmlPropElem = _xmlDoc.NewElement("Property");
            xmlPropElem->SetAttribute("type", asString(type).c_str());
            xmlPropElem->SetAttribute("name", name);
            bool skipAttribute = false;

            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    skipAttribute = true;
                    break;

                case IProperty::Type::Callback:
                {
                    skipAttribute = true;
                }
                break;

                case IProperty::Type::Uint8:
                    serializeIntegerPropertyToXML<u8>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::Uint16:
                    serializeIntegerPropertyToXML<u16>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::Uint32:
                    serializeIntegerPropertyToXML<u32>(_object, prop, xmlPropElem);
                break;

                case IProperty::Type::Uint64:
                    serializeIntegerPropertyToXML<u64>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::Resource:
                {
                    const IObject* pResource = prop->GetPropertyResource(_object);
                    serializeToXML(pResource, _xmlDoc, xmlPropElem);
                }
                break;

                case IProperty::Type::ObjectRef:
                {
                    const IObject * pObject = prop->GetPropertyObjectRef(_object);
                    serializeToXML(pObject, _xmlDoc, xmlPropElem);
                }
                break;

                case IProperty::Type::ObjectRefVector:
                {
                    auto * vector = prop->GetPropertyObjectRefVector(_object);
                    const uint count = vector->count();
                    for (uint i = 0; i < count; ++i)
                    {
                        serializeToXML((const IObject *)(*vector)[i], _xmlDoc, xmlPropElem);
                    }
                }
                break;

                case IProperty::Type::String:
                {
                    const string * pString = prop->GetPropertyString(_object);
                    xmlPropElem->SetAttribute("value", pString->c_str());
                }
                break;

                case IProperty::Type::Bool:
                {
                    const bool * pBool = prop->GetPropertyBool(_object);
                    xmlPropElem->SetAttribute("value", *pBool);
                }
                break;

                case IProperty::Type::Float:
                {
                    const float * pFloat = prop->GetPropertyFloat(_object);
                    xmlPropElem->SetAttribute("value", *pFloat);
                }
                break;

                case IProperty::Type::Float4:
                {
                    const float * pFloat4 = (const float *)prop->GetPropertyFloat4(_object);
                    xmlPropElem->SetAttribute("x", pFloat4[0]);
                    xmlPropElem->SetAttribute("y", pFloat4[1]);
                    xmlPropElem->SetAttribute("z", pFloat4[2]);
                    xmlPropElem->SetAttribute("w", pFloat4[3]);
                }
                break;

                case IProperty::Type::Float4x4:
                {
                    const float * pFloat4x4 = (const float *)(uint_ptr(_object) + offset);
                    xmlPropElem->SetAttribute("Ix", pFloat4x4[0 + 0]);
                    xmlPropElem->SetAttribute("Iy", pFloat4x4[0 + 1]);
                    xmlPropElem->SetAttribute("Iz", pFloat4x4[0 + 2]);
                    xmlPropElem->SetAttribute("Iw", pFloat4x4[0 + 3]);
                    xmlPropElem->SetAttribute("Jx", pFloat4x4[4 + 0]);
                    xmlPropElem->SetAttribute("Jy", pFloat4x4[4 + 1]);
                    xmlPropElem->SetAttribute("Jz", pFloat4x4[4 + 2]);
                    xmlPropElem->SetAttribute("Jw", pFloat4x4[4 + 3]);
                    xmlPropElem->SetAttribute("Kx", pFloat4x4[8 + 0]);
                    xmlPropElem->SetAttribute("Ky", pFloat4x4[8 + 1]);
                    xmlPropElem->SetAttribute("Kz", pFloat4x4[8 + 2]);
                    xmlPropElem->SetAttribute("Kw", pFloat4x4[8 + 3]);
                    xmlPropElem->SetAttribute("Tx", pFloat4x4[12 + 0]);
                    xmlPropElem->SetAttribute("Ty", pFloat4x4[12 + 1]);
                    xmlPropElem->SetAttribute("Tz", pFloat4x4[12 + 2]);
                    xmlPropElem->SetAttribute("Tw", pFloat4x4[12 + 3]);
                }
                break;

                case IProperty::Type::EnumFlagsU8:
                    serializeEnumFlagsPropertyToXML<u8>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumFlagsU16:
                    serializeEnumFlagsPropertyToXML<u16>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumFlagsU32:
                    serializeEnumFlagsPropertyToXML<u32>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumFlagsU64:
                    serializeEnumFlagsPropertyToXML<u64>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumU8:
                    serializeEnumPropertyToXML<u8>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumU16:
                    serializeEnumPropertyToXML<u16>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumU32:
                    serializeEnumPropertyToXML<u32>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumU64:
                    serializeEnumPropertyToXML<u64>(_object, prop, xmlPropElem);
                    break;
            }

            if (!skipAttribute)
                xmlElement->InsertEndChild(xmlPropElem);
        }

        parent->InsertEndChild(xmlElement);
        return true;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void Factory::serializeIntegerPropertyFromXML(IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const
    {
        const XMLAttribute * xmlValue = _xmlElem->FindAttribute("value");
        if (nullptr != xmlValue)
        {
            T * p = (T *)(uint_ptr(_object) + _prop->getOffset());
            *p = (T)xmlValue->Int64Value();
        }
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void Factory::serializeIntegerPropertyToXML(const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem) const
    {
        const T * p = (T *)(uint_ptr(_object) + _prop->getOffset());
        _xmlElem->SetAttribute("value", *p);
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void Factory::serializeEnumPropertyFromXML(IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const
    {
        T * pEnum = (T *)(uint_ptr(_object) + _prop->getOffset());
        const XMLAttribute * xmlValue = _xmlElem->FindAttribute("value");
        if (nullptr != xmlValue)
        {
            const auto val = xmlValue->Value();
            for (uint i = 0; i < _prop->getEnumCount(); ++i)
            {
                if (!strcmp(val, _prop->getEnumName(i)))
                    *pEnum = _prop->getEnumValue(i);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    template<typename T> void Factory::serializeEnumPropertyToXML(const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem) const
    {
        const T * pEnum = (const T *)(uint_ptr(_object) + _prop->getOffset());
        for (uint i = 0; i < _prop->getEnumCount(); ++i)
        {
            if (*pEnum == _prop->getEnumValue(i))
            {
                _xmlElem->SetAttribute("value", _prop->getEnumName(i));
                break;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void Factory::serializeEnumFlagsPropertyFromXML(IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const
    {
        T * pEnum = (T *)(uint_ptr(_object) + _prop->getOffset());
        T enumVal = 0;

        const XMLAttribute * xmlValue = _xmlElem->FindAttribute("value");
        if (nullptr != xmlValue)
        {
            auto xmlEnumString = xmlValue->Value();
            const char * p = xmlEnumString;

            do
            {
                char temp[256];
                temp[0] = '\0';
                int index = 0;
                do
                {
                    temp[index] = *p++;
                    index++;
                } while ('|' != *p && '\0' != *p);
                temp[index] = '\0';

                auto xmlEnumString = xmlValue->Value();
                for (uint e = 0; e < _prop->getEnumCount(); ++e)
                {
                    auto name = _prop->getEnumName(e);
                    auto value = _prop->getEnumValue(e);

                    if (!strcmp(name, temp))
                    {
                        enumVal |= value;
                        break;
                    }
                }
            } while ('\0' != *p++);
        }
        *pEnum = enumVal;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void Factory::serializeEnumFlagsPropertyToXML(const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem) const
    {
        const u32 * pEnum = (const u32 *)(uint_ptr(_object) + _prop->getOffset());
        string flags;
        bool first = true;
        for (uint i = 0; i < _prop->getEnumCount(); ++i)
        {
            if (0 != (*pEnum & _prop->getEnumValue(i)))
            {
                if (!first)
                    flags += "|";
                else
                    first = false;
                flags += (string)_prop->getEnumName(i);
            }
        }
        _xmlElem->SetAttribute("value", flags.c_str());
    }

    //--------------------------------------------------------------------------------------
    void Factory::ReleaseAsync(core::IObject * _object)
    {
        lock_guard<mutex> lock(m_objectsToReleaseMutex);
        m_objectsToRelease[m_objectsToReleaseTableIndex].push_back(_object);
    }

    //--------------------------------------------------------------------------------------
    void Factory::FlushReleaseAsync()
    {
        lock_guard<mutex> lock(m_objectsToReleaseMutex);

        auto & objectsToRelease = m_objectsToRelease[m_objectsToReleaseTableIndex ^ 1];

        if (objectsToRelease.size())
        {
            //VG_DEBUGPRINT("Release %u object(s) async\n", objectsToRelease.size());
            for (IObject * obj : objectsToRelease)
                VG_SAFE_RELEASE(obj);
            objectsToRelease.clear();
        }

        m_objectsToReleaseTableIndex ^= 1;
    }
}