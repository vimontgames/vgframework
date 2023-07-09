#include "core/Precomp.h"
#include "Factory.h"
#include "core/Math/Math.h"
#include "core/IResource.h"
#include "core/XML/XML.h"
#include "core/Timer/Timer.h"
#include "core/Kernel.h"

using namespace tinyxml2;

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Returned pointer shall not be stored but used immediatly ;)
    //--------------------------------------------------------------------------------------
    IClassDesc * Factory::registerClass(const char * _className, const char * _displayName, IClassDesc::Flags _flags, IClassDesc::Func _createFunc)
    {
        // Classes declared in shared static libs could be declared more than once at static init
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            ClassDesc & desc = m_classes[i];
            if (!strcmp(desc.getClassName(), _className))
                return nullptr; // already registered
        }

        VG_DEBUGPRINT("[ObjectFactory] Register class \"%s\"\n", _className);

        ClassDesc classDesc;
        classDesc.name = _className;
        classDesc.displayName = _displayName ? _displayName : _className;
        classDesc.flags = _flags;
        classDesc.createFunc = _createFunc;

        m_classes.push_back(classDesc);

        return &m_classes.back();
    }

    //--------------------------------------------------------------------------------------
    IClassDesc * Factory::registerSingletonClass(const char * _className, const char * _displayName, IClassDesc::Flags _flags, IClassDesc::SingletonFunc _singletonFunc)
    {
        // Classes declared in shared static libs could be declared more than once at static init
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            ClassDesc & desc = m_classes[i];
            if (!strcmp(desc.getClassName(), _className))
                return nullptr; // already registered
        }

        VG_DEBUGPRINT("[ObjectFactory] Register singleton class \"%s\"\n", _className);

        ClassDesc classDesc;
        classDesc.name = _className;
        classDesc.displayName = _displayName ? _displayName : _className;
        classDesc.flags = _flags;
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
    IObject * Factory::createFromXML(const string & _XMLfilename) const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::loadFromXML(IObject * _object, const string & _XMLfilename) const
    {
        const auto startLoad = Timer::getTick();
        const auto * factory = Kernel::getFactory();

        XMLDoc xmlDoc;
        if (XMLError::XML_SUCCESS == xmlDoc.LoadFile(_XMLfilename.c_str()))
        {
            XMLNode * xmlRoot = xmlDoc.FirstChild();
            if (xmlRoot != nullptr)
            {
                if (factory->serializeFromXML(_object, xmlDoc))
                {
                    VG_DEBUGPRINT("[Factory] Load and Parse XML \"%s\" ... %.2f ms\n", _XMLfilename.c_str(), Timer::getEnlapsedTime(startLoad, Timer::getTick()));
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
    bool Factory::serializeFromString(IObject * _object, const string & _in) const
    {
        string className;
        ClassDesc * desc = nullptr;
        map<string, string> values;

        if (!_in.empty())
        {
            size_t offset = 0;

            // get class name
            auto beginClassName = _in.find("[", offset);
            if (string::npos != beginClassName)
            {
                auto endClassName = _in.find("]", offset);
                if (string::npos != endClassName)
                {
                    className = _in.substr(beginClassName + 1, endClassName - 1);
                    offset = endClassName + 1;
                }
            }

            desc = (ClassDesc*)getClassDescriptor(className.c_str());
            if (nullptr == desc)
            {
                VG_ASSERT(false, "Cannot serialize unknown class \"%s\"", className.c_str());
                return false;
            }

            while (true)
            {
                auto keyOffset = max(_in.find_first_not_of("\r\n", offset), _in.find_first_not_of("\n", offset));

                if (string::npos != keyOffset)
                {
                    auto keyEndOffset = _in.find("=", offset);
                    string key = _in.substr(keyOffset, keyEndOffset - keyOffset);

                    auto beginOffset = _in.find("=", keyOffset);
                    auto endOffset = min(_in.find("\n", beginOffset + 2), _in.find("\r\n", beginOffset + 2));

                    if (string::npos != beginOffset && string::npos != endOffset)
                    {
                        string value = _in.substr(beginOffset + 1, endOffset - (beginOffset + 1));
                        values.insert({ key, value });

                        offset = endOffset + 1;
                    }
                }
                else
                    break;
            }
        }

        uint_ptr curOffset = (uint_ptr)-1;

        for (uint p = 0; p < desc->getPropertyCount(); ++p)
        {
            const auto & prop = desc->getPropertyByIndex(p);
            const auto * name = prop->getName();
            const auto type = prop->getType();
            const auto offset = prop->getOffset();

            // if several properties are serialized at the same offset (i.e. bitfields, enums, ...) then only load the first one
            if (offset == curOffset)
                continue;
            curOffset = offset;

            auto it = values.find(name);
            if (values.end() != it)
            {
                switch (type)
                {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    break;

                case IProperty::Type::Resource:
                {
                    IResource * pResource = prop->GetPropertyResource(_object);
                    pResource->setResourcePath(it->second.substr(1, it->second.length() - 2));
                }
                break;

                case IProperty::Type::EnumU32:
                {
                    u32 * pEnum = prop->GetPropertyUint32(_object);

                    string enumValName = it->second.substr(1, it->second.length() - 2);
                    u32 enumVal = 0;

                    for (uint q = p; q < desc->getPropertyCount(); ++q)
                    {
                        const auto & enumprop = desc->getPropertyByIndex(q);

                        if (offset == enumprop->getOffset())
                        {
                            if (enumValName == enumprop->getDisplayName())
                            {
                                enumVal = enumprop->getSizeOf();
                                break;
                            }
                        }
                    }

                    *pEnum = enumVal;
                }
                break;

                case IProperty::Type::Callback:
                    break;

                case IProperty::Type::Bool:
                {
                    bool * pBool = prop->GetPropertyBool(_object);
                    *pBool = (!strcmp(it->second.c_str(), "true") || !strcmp(it->second.c_str(), "TRUE") || !strcmp(it->second.c_str(), "1")) ? true : false;
                }
                break;

                case IProperty::Type::String:
                {
                    string * pString = prop->GetPropertyString(_object);
                    *pString = it->second.substr(1, it->second.length() - 2);
                }
                break;

                case IProperty::Type::Float4:
                {
                    float4 * pFloat4 = prop->GetPropertyFloat4(_object);
                    float f[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

                    auto beginFloat4 = _in.find("{", offset);
                    if (string::npos != beginFloat4)
                    {
                        auto cur = beginFloat4;

                        auto endFloat4 = _in.find("}", cur);
                        if (string::npos != endFloat4)
                        {
                            for (uint i = 0; i < 4; ++i)
                            {
                                auto endFloat = min(_in.find(",", cur), endFloat4);
                                const string val = _in.substr(cur + 1, endFloat - cur - 1);
                                f[i] = stof(val);
                                cur = endFloat + 1;
                            }
                        }
                    }

                    *pFloat4 = float4(f[0], f[1], f[2], f[3]);
                }
                break;
                }
            }
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::serializeToString(string & _out, const IObject * _object) const
    {
        const char * className = _object->getClassName();
        _out += (string)"<" + className + ">" + "\n";

        const auto * classDesc = getClassDescriptor(className);

        uint_ptr curOffset = (uint_ptr)-1;

        for (uint p = 0; p < classDesc->getPropertyCount(); ++p)
        {
            const auto & prop = classDesc->getPropertyByIndex(p);

            const auto name = prop->getName();
            const auto type = prop->getType();
            const auto offset = prop->getOffset();

            // if several properties are serialized at the same offset (i.e. bitfields, enums, ...) then only write the first one
            if (offset == curOffset)
                continue;
            curOffset = offset;

            string value;

            switch (type)
            {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                break;

            case IProperty::Type::ObjectRef:
            {
                IObject * pObject = prop->GetPropertyObjectRef(_object);
                _out += (string)name + "=";
                serializeToString(_out, pObject);
            }
            break;

            case IProperty::Type::Resource:
            {
                IResource * pResource = prop->GetPropertyResource(_object);
                value = "\"" + pResource->getResourcePath() + "\"";
            }
            break;

            case IProperty::Type::Callback:
                continue;

            case IProperty::Type::EnumFlagsU32:
            {
                u32 * pEnum = prop->GetPropertyUint32(_object);
                value = to_string(*pEnum);
            };

            case IProperty::Type::EnumU32:
            {
                u32 * pEnum = prop->GetPropertyUint32(_object);

                string enumValName = "";

                for (uint q = p; q < classDesc->getPropertyCount(); ++q)
                {
                    const auto & enumprop = classDesc->getPropertyByIndex(q);

                    if (offset == enumprop->getOffset() && *pEnum == enumprop->getSizeOf())
                    {
                        enumValName = enumprop->getDisplayName();
                        break;
                    }
                }

                value = "\"" + enumValName + "\"";
            }
            break;

            case IProperty::Type::Bool:
                value = *prop->GetPropertyBool(_object) ? "true" : "false";
                break;

            case IProperty::Type::String:
                value = "\"" + *prop->GetPropertyString(_object) + "\"";
                break;

            case IProperty::Type::Float4:
            {
                float4 * v = prop->GetPropertyFloat4(_object);
                value = "{" + to_string(v->x) + ", " + to_string(v->y) + ", " + to_string(v->z) + ", " + to_string(v->w) + "}";
            }
            break;
            }

            _out += (string)name + "=" + value + "\n";
        }

        _out += (string)"</" + className + ">" + "\n";

        return true;
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

                                    case IProperty::Type::EnumFlagsU32:
                                    {
                                        u32 * pEnum = (u32*)(uint_ptr(_object) + offset);
                                        u32 enumVal = 0;

                                        const XMLAttribute* xmlValue = xmlPropElem->FindAttribute("value");
                                        if (nullptr != xmlValue)
                                        {
                                            auto xmlEnumString = xmlValue->Value();
                                            for (uint e = 0; e < prop->getEnumCount(); ++e)
                                            {
                                                auto name = prop->getEnumName(e);
                                                auto value = prop->getEnumValue(e);

                                                if (!strcmp(name, xmlEnumString))
                                                    enumVal |= value;
                                            }
                                        }
                                        *pEnum = enumVal;
                                    }
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

                                    case IProperty::Type::EnumU32:
                                    {
                                        u32 * pEnum = (u32*)(uint_ptr(_object) + offset);
                                        const XMLAttribute * xmlValue = xmlPropElem->FindAttribute("value");
                                        if (nullptr != xmlValue)
                                        {
                                            const u32 val = xmlValue->UnsignedValue();
                                            for (uint i = 0; i < prop->getEnumCount(); ++i)
                                            {
                                                if (val == prop->getEnumValue(i))
                                                    *pEnum = val;
                                            }
                                        }
                                    }
                                    }
                                }
                                else
                                {
                                    VG_DEBUGPRINT("[ObjectFactory] Serialized Object type \"%s\" for Property \"%s\" from class \"%s\" does not match type \"%s\" declared in ClassDesc\n", typeName, name, className, asString(prop->getType()));
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
                    serializeToXML((const IObject*)(*vector)[i], _xmlDoc, xmlPropElem);
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
                const float * pFloat4 = (const float*)prop->GetPropertyFloat4(_object);
                xmlPropElem->SetAttribute("x", pFloat4[0]);
                xmlPropElem->SetAttribute("y", pFloat4[1]);
                xmlPropElem->SetAttribute("z", pFloat4[2]);
                xmlPropElem->SetAttribute("w", pFloat4[3]);
            }
            break;

            case IProperty::Type::Float4x4:
            {
                const float * pFloat4x4 = (const float*)(uint_ptr(_object) + offset);
                xmlPropElem->SetAttribute("Ix", pFloat4x4[0+0]);
                xmlPropElem->SetAttribute("Iy", pFloat4x4[0+1]);
                xmlPropElem->SetAttribute("Iz", pFloat4x4[0+2]);
                xmlPropElem->SetAttribute("Iw", pFloat4x4[0+3]);
                xmlPropElem->SetAttribute("Jx", pFloat4x4[4+0]);
                xmlPropElem->SetAttribute("Jy", pFloat4x4[4+1]);
                xmlPropElem->SetAttribute("Jz", pFloat4x4[4+2]);
                xmlPropElem->SetAttribute("Jw", pFloat4x4[4+3]);
                xmlPropElem->SetAttribute("Kx", pFloat4x4[8+0]);
                xmlPropElem->SetAttribute("Ky", pFloat4x4[8+1]);
                xmlPropElem->SetAttribute("Kz", pFloat4x4[8+2]);
                xmlPropElem->SetAttribute("Kw", pFloat4x4[8+3]);
                xmlPropElem->SetAttribute("Tx", pFloat4x4[12+0]);
                xmlPropElem->SetAttribute("Ty", pFloat4x4[12+1]);
                xmlPropElem->SetAttribute("Tz", pFloat4x4[12+2]);
                xmlPropElem->SetAttribute("Tw", pFloat4x4[12+3]);
            }
            break;

            case IProperty::Type::EnumFlagsU32:
            {
                const u32 * pEnum = (const u32*)(uint_ptr(_object) + offset);
                string flags;
                bool first = true;
                for (uint i = 0; i < prop->getEnumCount(); ++i)
                {
                    if (*pEnum == prop->getEnumValue(i))
                    {
                        if (!first)
                            flags += "|";
                        else
                            first = false;
                        flags += (string)prop->getEnumName(i);
                    }
                }
                xmlPropElem->SetAttribute("value", flags.c_str());
            }
            break;

            case IProperty::Type::EnumU32:
            {
                const u32 * pEnum = (const u32*)(uint_ptr(_object) + offset);
                for (uint i = 0; i < prop->getEnumCount(); ++i)
                {
                    if (*pEnum == prop->getEnumValue(i))
                        xmlPropElem->SetAttribute("value", prop->getEnumName(i));
                }
            }
            break;
            }

            if (!skipAttribute)
                xmlElement->InsertEndChild(xmlPropElem);
        }

        parent->InsertEndChild(xmlElement);
        return true;
    }
}