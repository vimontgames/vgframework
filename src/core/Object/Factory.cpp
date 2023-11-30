#include "core/Precomp.h"
#include "Factory.h"
#include "core/Math/Math.h"
#include "core/IResource.h"
#include "core/XML/XML.h"
#include "core/Timer/Timer.h"
#include "core/Kernel.h"
#include "engine/IResourceManager.h"
#include "core/File/File.h"
#include "core/File/Buffer.h"

using namespace tinyxml2;

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Factory::Factory()
    {

    }

    //--------------------------------------------------------------------------------------
    Factory::~Factory()
    {
        for (auto val : m_initValues)
        {
            VG_SAFE_DELETE(val.second);
        }
        m_initValues.clear();
    }

    //--------------------------------------------------------------------------------------
    // Returned pointer shall not be stored but used immediately
    //--------------------------------------------------------------------------------------
    IClassDesc * Factory::registerClass(const char * _interfaceName, const char * _className, const char * _classDisplayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::Func _createFunc)
    {
        // Classes declared in shared static libs could be declared more than once at static init
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            ClassDesc & desc = m_classes[i];
            if (!strcmp(desc.GetClassName(), _className))
            {
                VG_INFO("[Factory] Class \"%s\" is already registered", _className);
                return nullptr; 
            }
        }

        // Use parent class name as interface name if it starts with an 'I'
        // Note that it does not handle hierarchy of multiple interfaces but for now it's 
        // enough e.g. to return an "AnimationComponent" object when requesting an "IAnimationComponent"
        // but it won't return it when requesting e.g. an "IObject" despite being in the inheritance chain
        if (nullptr != _interfaceName && _interfaceName[0] == 'I')
        {
            VG_INFO("[Factory] Register class \"%s\" (%s)", _className, _interfaceName);
        }
        else
        {
            VG_INFO("[Factory] Register class \"%s\"", _className);
            _interfaceName = nullptr;
        }        

        ClassDesc classDesc;
        classDesc.name = _className;
        classDesc.interfaceName = _interfaceName;
        classDesc.displayName = _classDisplayName ? _classDisplayName : _className;
        classDesc.flags = _flags;
        classDesc.sizeOf = sizeOf;
        classDesc.createFunc = _createFunc;

        m_classes.push_back(classDesc);

        return &m_classes.back();
    }

    //--------------------------------------------------------------------------------------
    IClassDesc * Factory::registerSingletonClass(const char * _interfaceName, const char * _className, const char * _classDisplayName, IClassDesc::Flags _flags, u32 sizeOf, IClassDesc::SingletonFunc _singletonFunc)
    {
        // Classes declared in shared static libs could be declared more than once at static init
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            ClassDesc & desc = m_classes[i];
            if (!strcmp(desc.GetClassName(), _className))
            {
                VG_INFO("[Factory] Singleton Class \"%s\" is already registered", _className);
                return nullptr;
            }
        }

        VG_INFO("[Factory] Register singleton class \"%s\"", _className);

        ClassDesc classDesc;
        classDesc.name = _className;
        classDesc.displayName = _classDisplayName ? _classDisplayName : _className;
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
            VG_ASSERT(asBool(IClassDesc::Flags::Singleton & desc->GetFlags()));
            return desc->GetSingletonFunc()();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    IObject * Factory::createObject(const char * _className, const string & _name, IObject * _parent) const
    {
        auto * desc = getClassDescriptor(_className);

        if (desc)
        {
            VG_ASSERT(!asBool(IClassDesc::Flags::Singleton & desc->GetFlags()));
            IObject * obj = desc->GetCreateFunc()(_name, _parent);

            const auto index = desc->GetNextIndex();

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
                //VG_INFO("[Factory] Load \"%s\"", relativePath.c_str());
                if (factory->serializeFromXML(_object, xmlDoc))
                {
                    VG_INFO("[Factory] \"%s\" loaded from XML in %.2f ms", relativePath.c_str(), Timer::getEnlapsedTime(startLoad, Timer::getTick()));
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
    bool Factory::SaveProperties(core::IObject * _object)
    {
        auto it = m_initValues.find(_object);
        VG_ASSERT((it == m_initValues.end()));
        if (it == m_initValues.end())
        {
            io::Buffer * buffer = new io::Buffer();
            it = m_initValues.insert(std::pair(_object, buffer)).first;
            return serializeToMemory(_object, *buffer);
        }
        
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::RestoreProperties(core::IObject * _object)
    {
        auto it = m_initValues.find(_object);
        if (it != m_initValues.end())
        {
            io::Buffer * buffer = it->second;
            bool result = serializeFromMemory(_object, *buffer);
            VG_SAFE_DELETE(it->second);
            m_initValues.erase(it);
            return result;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::serializeToMemory(const IObject * _object, io::Buffer & _buffer)
    {
        const char * className = _object->getClassName();
        const auto * classDesc = getClassDescriptor(className);

        for (uint p = 0; p < classDesc->GetPropertyCount(); ++p)
        {
            const auto & prop = classDesc->GetPropertyByIndex(p);

            const auto name = prop->getName();
            const auto type = prop->getType();
            const auto size = prop->getSizeOf();
            const auto offset = prop->getOffset();
            const auto flags = prop->getFlags();
            const bool isEnumArray = asBool(IProperty::Flags::EnumArray & prop->getFlags());

            switch (type)
            {
                default:
                    //VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    break;

                case IProperty::Type::Callback:
                    break;

                case IProperty::Type::Bool:
                case IProperty::Type::Int8:
                case IProperty::Type::Int16:
                case IProperty::Type::Int32:
                case IProperty::Type::Int64:
                case IProperty::Type::Uint8:
                case IProperty::Type::Uint16:
                case IProperty::Type::Uint32:
                case IProperty::Type::Uint64:
                case IProperty::Type::Float:
                case IProperty::Type::Float2:
                case IProperty::Type::Float3:
                case IProperty::Type::Float4:
                case IProperty::Type::Float4x4:
                case IProperty::Type::EnumU8:
                case IProperty::Type::EnumU16:
                case IProperty::Type::EnumU32:
                case IProperty::Type::EnumU64:
                case IProperty::Type::EnumFlagsU8:
                case IProperty::Type::EnumFlagsU16:
                case IProperty::Type::EnumFlagsU32:
                case IProperty::Type::EnumFlagsU64:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization from Memory not implemented for type '%s'", asString(type).c_str());
                    const void * src = (void *)(uint_ptr(_object) + offset);
                    VG_VERIFY(_buffer.write(src, size));
                }
            }
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::serializeFromMemory(IObject * _object, io::Buffer & _buffer)
    {
        const char * className = _object->getClassName();
        const auto * classDesc = getClassDescriptor(className);

        for (uint p = 0; p < classDesc->GetPropertyCount(); ++p)
        {
            const auto & prop = classDesc->GetPropertyByIndex(p);

            const char * name = prop->getName();
            const auto type = prop->getType();
            const auto size = prop->getSizeOf();
            const auto offset = prop->getOffset();
            const auto flags = prop->getFlags();
            const bool isEnumArray = asBool(IProperty::Flags::EnumArray & prop->getFlags());

            switch (type)
            {
                default:
                    //VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    break;

                case IProperty::Type::Callback:
                    break;

                case IProperty::Type::Bool:
                case IProperty::Type::Int8:
                case IProperty::Type::Int16:
                case IProperty::Type::Int32:
                case IProperty::Type::Int64:
                case IProperty::Type::Uint8:
                case IProperty::Type::Uint16:
                case IProperty::Type::Uint32:
                case IProperty::Type::Uint64:
                case IProperty::Type::Float:
                case IProperty::Type::Float2:
                case IProperty::Type::Float3:
                case IProperty::Type::Float4:
                case IProperty::Type::Float4x4:
                case IProperty::Type::EnumU8:
                case IProperty::Type::EnumU16:
                case IProperty::Type::EnumU32:
                case IProperty::Type::EnumU64:
                case IProperty::Type::EnumFlagsU8:
                case IProperty::Type::EnumFlagsU16:
                case IProperty::Type::EnumFlagsU32:
                case IProperty::Type::EnumFlagsU64:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization from Memory not implemented for type '%s'", asString(type).c_str());
                    void * dst = (void*)(uint_ptr(_object) + offset);

                    bool changed = false;
                    VG_VERIFY(_buffer.restore(dst, size, changed));
                    if (changed)
                        VG_INFO("[Factory] Property (%s) '%s' from %s \"%s\" has been restored", asString(type).c_str(), name, _object->getClassName(), _object->getName().c_str());
                }
            }
        }

        return true;
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

    template <uint N> class AliasType
    {
        u8 data[N>>3];
    };

    template <uint N> struct UnknownElement
    {
        u8 m_data[N>>3];
    };

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
                            const auto * prop = classDesc->GetPropertyByName(name);
                            if (nullptr == prop)
                            {
                                VG_WARNING("[Factory] Class \"%s\" has no property \"%s\" of type '%s'", className, name, typeName);
                            }
                            else
                            {
                                const bool isEnumArray = asBool(IProperty::Flags::EnumArray & prop->getFlags());

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
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
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
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
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

                                        case IProperty::Type::Object:
                                        {
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            IObject * pObjectRef = (IObject *)(uint_ptr(_object) + offset);
                                            const XMLElement * xmlObjectRef = xmlPropElem->FirstChildElement("Object");
                                            if (nullptr != xmlObjectRef)
                                            {
                                                const XMLAttribute * xmlClassAttrRef = xmlObjectRef->FindAttribute("class");
                                                if (nullptr != xmlClassAttrRef)
                                                {
                                                    const char * classNameRef = xmlClassAttrRef->Value();
                                                    const auto * classDescRef = getClassDescriptor(classNameRef);
                                                    if (nullptr != classDescRef)
                                                    {
                                                        // TODO: clear contents?
                                                        VG_VERIFY(serializeFromXML(pObjectRef, xmlObjectRef));
                                                        pObjectRef->setParent(_object);
                                                    }
                                                }
                                            }
                                        }
                                        break;

                                        case IProperty::Type::Resource:
                                        case IProperty::Type::ResourceRef:
                                        {
                                            const bool ref = type == IProperty::Type::ResourceRef;

                                            auto serializeResourceFromXML = [=](IResource * _resource, const XMLElement * _xmlElement)
                                            {
                                                const IClassDesc * classDescRef = nullptr;
                                                const XMLElement * xmlObjectRef = _xmlElement->FirstChildElement("Object");
                                                if (nullptr != xmlObjectRef)
                                                {
                                                    const XMLAttribute * xmlClassAttrRef = xmlObjectRef->FindAttribute("class");
                                                    if (nullptr != xmlClassAttrRef)
                                                    {
                                                        const char * classNameRef = xmlClassAttrRef->Value();
                                                        classDescRef = getClassDescriptor(classNameRef);
                                                        if (nullptr != classDescRef)
                                                        {
                                                            if (serializeFromXML(_resource, xmlObjectRef))
                                                            {
                                                                _resource->setParent(_object);
                                                                _resource->onResourcePathChanged("", _resource->GetResourcePath());
                                                            }
                                                        }
                                                    }
                                                }
                                            };

                                            if (isEnumArray)
                                            {
                                                const XMLElement * xmlPropElemValue = xmlPropElem->FirstChildElement("Value");
                                                if (nullptr != xmlPropElemValue)
                                                {
                                                    do
                                                    {
                                                        const XMLAttribute * xmlValueName = xmlPropElemValue->FindAttribute("name");
                                                        if (nullptr != xmlValueName)
                                                        {
                                                            for (uint i = 0; i < prop->getEnumCount(); ++i)
                                                            {
                                                                if (!strcmp(xmlValueName->Value(), prop->getEnumName(i)))
                                                                {
                                                                    IResource * pResource = ref ? prop->GetPropertyResourceRef(_object, i) : prop->GetPropertyResource(_object, i);
                                                                    serializeResourceFromXML(pResource, xmlPropElemValue);
                                                                    break;
                                                                }
                                                            }
                                                        }

                                                        xmlPropElemValue = xmlPropElemValue->NextSiblingElement("Value");
                                                    } while (nullptr != xmlPropElemValue);
                                                }
                                            }
                                            else
                                            {
                                                IResource * pResource = ref ? prop->GetPropertyResourceRef(_object) : prop->GetPropertyResource(_object);
                                                serializeResourceFromXML(pResource, xmlPropElem);
                                            }
                                        }
                                        break;

                                        case IProperty::Type::ResourceVector:
                                        {
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            const XMLElement * const xmlObjectRefStart = xmlPropElem->FirstChildElement("Object");
                                            if (nullptr != xmlObjectRefStart)
                                            {
                                                // first loop to get type and element count
                                                auto xmlObjectRef = xmlObjectRefStart;
                                                uint count = 0;
                                                const IClassDesc * classDesc = nullptr;
                                                do
                                                {
                                                    const XMLAttribute * xmlClassAttrRef = xmlObjectRef->FindAttribute("class");
                                                    if (nullptr != xmlClassAttrRef)
                                                    {
                                                        const char * classNameRef = xmlClassAttrRef->Value();
                                                        const auto * classDescRef = getClassDescriptor(classNameRef);
                                                        if (nullptr != classDescRef)
                                                        {
                                                            count++;
                                                            VG_ASSERT(classDesc == nullptr || classDesc == classDescRef);
                                                            classDesc = classDescRef;
                                                        }
                                                    }
                                                    xmlObjectRef = xmlObjectRef->NextSiblingElement("Object");

                                                } while (xmlObjectRef != nullptr);

                                                uint elemSize = 0;
                                                void * data = classDesc->ResizeVector(_object, (uint)offset, count, elemSize);
                                                VG_ASSERT(nullptr != data);

                                                if (nullptr != data)
                                                {
                                                    // second pass to populate array
                                                    xmlObjectRef = xmlObjectRefStart;
                                                    uint index = 0;
                                                    do
                                                    {
                                                        const XMLAttribute * xmlClassAttrRef = xmlObjectRef->FindAttribute("class");
                                                        if (nullptr != xmlClassAttrRef)
                                                        {
                                                            const char * classNameRef = xmlClassAttrRef->Value();
                                                            const auto * classDescRef = getClassDescriptor(classNameRef);
                                                            if (nullptr != classDescRef)
                                                            {
                                                                IResource * pResource = (IResource *)(uint_ptr(data) + index * elemSize);
                                                                if (serializeFromXML(pResource, xmlObjectRef))
                                                                {
                                                                    pResource->setParent(_object);
                                                                    pResource->onResourcePathChanged("", pResource->GetResourcePath());
                                                                }
                                                                index++;
                                                            }
                                                        }

                                                        xmlObjectRef = xmlObjectRef->NextSiblingElement("Object");

                                                    } while (xmlObjectRef != nullptr);
                                                    VG_ASSERT(index == count);
                                                }
                                            }
                                        }
                                        break;

                                        case IProperty::Type::ObjectRef:
                                        {
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
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
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
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

                                        case IProperty::Type::EnumFlagsU8:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumFlagsPropertyFromXML<u8>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::EnumFlagsU16:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumFlagsPropertyFromXML<u16>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::EnumFlagsU32:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumFlagsPropertyFromXML<u32>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::EnumFlagsU64:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumFlagsPropertyFromXML<u64>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::String:
                                        {
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
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
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
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
                                            auto serializeFloat4FromXML = [](float4 * _value, const XMLElement * _xmlElement)
                                            {
                                                float * pFloat = (float *)_value;
                                                const char * values[] = { "x", "y", "z", "w" };
                                                for (uint i = 0; i < countof(values); ++i)
                                                {
                                                    const XMLAttribute * xmlValue = _xmlElement->FindAttribute(values[i]);
                                                    if (nullptr != xmlValue)
                                                        pFloat[i] = xmlValue->FloatValue();
                                                }
                                            };

                                            if (isEnumArray)
                                            {
                                                const XMLElement * xmlPropElemValue = xmlPropElem->FirstChildElement("Value");
                                                if (nullptr != xmlPropElemValue)
                                                {
                                                    do
                                                    {
                                                        const XMLAttribute * xmlValueName = xmlPropElemValue->FindAttribute("name");
                                                        if (nullptr != xmlValueName)
                                                        {
                                                            for (uint i = 0; i < prop->getEnumCount(); ++i)
                                                            {
                                                                if (!strcmp(xmlValueName->Value(), prop->getEnumName(i)))
                                                                {
                                                                    float4 * pFloat4 = prop->GetPropertyFloat4(_object, i);
                                                                    serializeFloat4FromXML(pFloat4, xmlPropElemValue);
                                                                    break;
                                                                }
                                                            }
                                                        }

                                                        xmlPropElemValue = xmlPropElemValue->NextSiblingElement("Value");
                                                    } while (nullptr != xmlPropElemValue);
                                                }
                                            }
                                            else
                                            {
                                                float4 * pFloat4 = prop->GetPropertyFloat4(_object);
                                                serializeFloat4FromXML(pFloat4, xmlPropElem);
                                            }
                                        }
                                        break;

                                        case IProperty::Type::EnumU8:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumPropertyFromXML<u8>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::EnumU16:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumPropertyFromXML<u16>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::EnumU32:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumPropertyFromXML<u32>(_object, prop, xmlPropElem);
                                        break;

                                        case IProperty::Type::Uint8:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<u8>(_object, prop, xmlPropElem);
                                        break;

                                        case IProperty::Type::Uint16:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<u16>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::Uint32:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<u32>(_object, prop, xmlPropElem);
                                            break;

                                        case IProperty::Type::Uint64:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<u64>(_object, prop, xmlPropElem);
                                            break;
                                    }
                                }
                                else
                                {
                                    VG_WARNING("[Factory] Serialized Object type \"%s\" for Property \"%s\" from class \"%s\" does not match type \"%s\" declared in ClassDesc\n", typeName, name, className, asString(prop->getType()));
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

        for (uint p = 0; p < classDesc->GetPropertyCount(); ++p)
        {
            const auto & prop = classDesc->GetPropertyByIndex(p);

            const auto name = prop->getName();
            const auto type = prop->getType();
            const auto offset = prop->getOffset();
            const auto flags = prop->getFlags();

            if (asBool(IProperty::Flags::NotSaved & flags))
                continue;

            XMLElement * xmlPropElem = _xmlDoc.NewElement("Property");
            xmlPropElem->SetAttribute("type", asString(type).c_str());
            xmlPropElem->SetAttribute("name", name);

            string flagsString = "";
            if (asBool(flags))
            {
                flagsString += asString(flags);
                xmlPropElem->SetAttribute("flags", flagsString.c_str());
            }

            bool skipAttribute = false;

            const bool isEnumArray = asBool(IProperty::Flags::EnumArray & prop->getFlags());

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
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeIntegerPropertyToXML<u8>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::Uint16:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeIntegerPropertyToXML<u16>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::Uint32:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeIntegerPropertyToXML<u32>(_object, prop, xmlPropElem);
                break;

                case IProperty::Type::Uint64:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeIntegerPropertyToXML<u64>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::Resource:
                case IProperty::Type::ResourceRef:
                {
                    const bool ref = type == IProperty::Type::ResourceRef;

                    if (isEnumArray)
                    {
                        const uint count = prop->getEnumCount();
                        for (uint i = 0; i < count; ++i)
                        {
                            XMLElement * xmlPropElemChild = _xmlDoc.NewElement("Value");
                            {
                                auto enumValueName = prop->getEnumName(i);
                                xmlPropElemChild->SetAttribute("name", enumValueName);
                                const IObject * pResource = ref ? prop->GetPropertyResourceRef(_object, i) : prop->GetPropertyResource(_object, i);
                                serializeToXML(pResource, _xmlDoc, xmlPropElemChild);
                            }
                            xmlPropElem->InsertEndChild(xmlPropElemChild);
                        }
                    }
                    else
                    {
                        const IObject * pResource = ref ? prop->GetPropertyResourceRef(_object) : prop->GetPropertyResource(_object);
                        serializeToXML(pResource, _xmlDoc, xmlPropElem);
                    }
                }
                break;

                case IProperty::Type::ResourceVector:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    const uint sizeOf = prop->getSizeOf();
                    const size_t count = prop->GetPropertyResourceVectorCount(_object);
                    const byte * data = prop->GetPropertyResourceVectorData(_object);

                    for (uint i = 0; i < count; ++i)
                    {
                        IObject * pObject = (IObject *)(data + sizeOf * i);
                        serializeToXML((const IObject *)(pObject), _xmlDoc, xmlPropElem);
                    }
                }
                break;

                case IProperty::Type::Object:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    const IObject * pObject = prop->GetPropertyObject(_object);
                    serializeToXML(pObject, _xmlDoc, xmlPropElem);
                }
                break;

                case IProperty::Type::ObjectRef:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    const IObject * pObject = prop->GetPropertyObjectRef(_object);
                    if (nullptr != pObject)
                        serializeToXML(pObject, _xmlDoc, xmlPropElem);
                }
                break;

                //case IProperty::Type::ObjectVector:
                //{
                //    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                //    const uint sizeOf = prop->getSizeOf();
                //    const size_t count = prop->GetPropertyObjectVectorCount(_object);
                //    const byte * data = prop->GetPropertyObjectVectorData(_object);
                //
                //    for (uint i = 0; i < count; ++i)
                //    {
                //        IObject * pObject = (IObject *)(data + sizeOf * i);
                //        serializeToXML((const IObject *)(pObject), _xmlDoc, xmlPropElem);
                //    }
                //}
                //break;

                case IProperty::Type::ObjectRefVector:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
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
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    const string * pString = prop->GetPropertyString(_object);
                    xmlPropElem->SetAttribute("value", pString->c_str());
                }
                break;

                case IProperty::Type::Bool:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    const bool * pBool = prop->GetPropertyBool(_object);
                    xmlPropElem->SetAttribute("value", *pBool);
                }
                break;

                case IProperty::Type::Float:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    const float * pFloat = prop->GetPropertyFloat(_object);
                    xmlPropElem->SetAttribute("value", *pFloat);
                }
                break;

                case IProperty::Type::Float4:
                {
                    auto serializeFloat4ToXML = [](XMLElement * _xmlElement, const float4 * _value)
                    {
                        const auto pFloat = (const float *)_value;
                        _xmlElement->SetAttribute("x", pFloat[0]);
                        _xmlElement->SetAttribute("y", pFloat[1]);
                        _xmlElement->SetAttribute("z", pFloat[2]);
                        _xmlElement->SetAttribute("w", pFloat[3]);
                    };

                    if (isEnumArray)
                    {
                        const uint count = prop->getEnumCount();
                        for (uint i = 0; i < count; ++i)
                        {
                            XMLElement * xmlPropElemChild = _xmlDoc.NewElement("Value");
                            {
                                auto enumValueName = prop->getEnumName(i);
                                xmlPropElemChild->SetAttribute("name", enumValueName);
                                const auto value = prop->GetPropertyFloat4(_object, i);
                                serializeFloat4ToXML(xmlPropElemChild, value);
                            }
                            xmlPropElem->InsertEndChild(xmlPropElemChild);
                        }
                    }
                    else
                    {
                        const auto value = prop->GetPropertyFloat4(_object);
                        serializeFloat4ToXML(xmlPropElem, value);
                    }
                }
                break;

                case IProperty::Type::Float4x4:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());

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
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumFlagsPropertyToXML<u8>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumFlagsU16:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumFlagsPropertyToXML<u16>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumFlagsU32:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumFlagsPropertyToXML<u32>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumFlagsU64:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumFlagsPropertyToXML<u64>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumU8:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumPropertyToXML<u8>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumU16:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumPropertyToXML<u16>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumU32:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumPropertyToXML<u32>(_object, prop, xmlPropElem);
                    break;

                case IProperty::Type::EnumU64:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
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
                    *pEnum = (T)_prop->getEnumValue(i); 
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