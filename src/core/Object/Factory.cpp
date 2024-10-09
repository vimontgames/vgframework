#include "core/Precomp.h"
#include "Factory.h"
#include "core/Math/Math.h"
#include "core/IResource.h"
#include "core/XML/XML.h"
#include "core/Timer/Timer.h"
#include "core/Kernel.h"
#include "core/File/File.h"
#include "core/File/Buffer.h"
#include "core/Misc/BitMask/BitMask.h"
#include "core/IInstance.h"
#include "core/Object/ObjectHandle.h"
#include "core/Types/Traits.h"

#include <random>

using namespace tinyxml2;

namespace vg::core
{
    static const uint classDescMaxCount = 1024;

    //--------------------------------------------------------------------------------------
    Factory::Factory()
    {
        // TODO : make proper API to register deprecated type names
        m_oldTypeNames.insert(std::pair("ResourceRef", PropertyType::ResourcePtr));
        m_oldTypeNames.insert(std::pair("ResourceRefVector", PropertyType::ResourcePtrVector));
        m_oldTypeNames.insert(std::pair("ObjectRef", PropertyType::ObjectPtr));
        m_oldTypeNames.insert(std::pair("ObjectRefVector", PropertyType::ObjectPtrVector));
        m_oldTypeNames.insert(std::pair("ObjectRefDictionary", PropertyType::ObjectPtrDictionary));

        // TODO : make proper API to register deprecated property names names for each class
        m_oldPropertyNames.insert(std::pair("m_world", "m_local"));
        m_oldPropertyNames.insert(std::pair("m_radius", "m_bottomRadius"));
        m_oldPropertyNames.insert(std::pair("m_materialResources", "m_resources"));
        m_oldPropertyNames.insert(std::pair("m_animationResources", "m_resources"));        

        // TODO: make proper API blah
        m_oldClassNames.insert(std::pair("RigidBodyComponent", "PhysicsBodyComponent"));
        m_oldClassNames.insert(std::pair("RigidBodyDesc", "PhysicsBodyDesc"));

        // Resizing m_classes would change class descriptor addresses
        m_classes.reserve(classDescMaxCount);
    }

    //--------------------------------------------------------------------------------------
    const char * Factory::fixDeprecatedClassName(const char * _className) const
    {
        auto it = m_oldClassNames.find(_className);
        if (m_oldClassNames.end() != it)
        {
            VG_WARNING("[Factory] Deprecated class name \"%s\" replaced by \"%s\"", it->first.c_str(), it->second);
            return it->second;
        }
        return _className;
    }

    //--------------------------------------------------------------------------------------
    Factory::~Factory()
    {
        for (uint i = 0; i < enumCount< BufferType>(); ++i)
        {
            auto & buffers = m_buffers[i];
            for (auto val : buffers)
                VG_SAFE_DELETE(val.second);
            buffers.clear();
        }
    }

    //--------------------------------------------------------------------------------------
    // Returned pointer shall not be stored but used immediately
    //--------------------------------------------------------------------------------------
    IClassDesc * Factory::RegisterObjectClass(const char * _parentClassName, const char * _className, const char * _classDisplayName, ClassDescFlags _flags, u32 sizeOf, IClassDesc::Func _createFunc)
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

        VG_ASSERT(m_classes.size() < classDescMaxCount);

        ClassDesc classDesc;
        classDesc.name = _className;
        classDesc.parentClassName = _parentClassName;
        classDesc.displayName = _classDisplayName ? _classDisplayName : _className;
        classDesc.flags = _flags;
        classDesc.sizeOf = sizeOf;
        classDesc.createFunc = _createFunc;

        m_classes.push_back(classDesc);

        return &m_classes.back();
    }

    //--------------------------------------------------------------------------------------
    IClassDesc * Factory::RegisterSingletonClass(const char * _parentClassName, const char * _className, const char * _classDisplayName, ClassDescFlags _flags, u32 sizeOf, IClassDesc::SingletonFunc _singletonFunc)
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
    bool Factory::IsRegisteredClass(const char * _className) const
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
    IObject * Factory::GetSingleton(const char * _className) const
    {
        const auto * desc = GetClassDescriptor(_className);
        if (desc)
        {
            VG_ASSERT(asBool(ClassDescFlags::Singleton & desc->GetFlags()));
            return desc->GetSingletonFunc()();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    IObject * Factory::CreateObject(const char * _className, const string & _name, IObject * _parent) const
    {
        auto * desc = GetClassDescriptor(_className);

        if (desc)
        {
            VG_ASSERT(!asBool(ClassDescFlags::Singleton & desc->GetFlags()));
            IObject * obj = desc->GetCreateFunc()(_name, _parent);

            const auto index = desc->GetNextIndex();

            if (obj->GetName().length() == 0)
            {
                if (_name.length())
                    obj->SetName(_name);
                else
                    obj->SetName((string)obj->GetClassName() + " #" + to_string(index));
            }

            return obj;
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::LoadFromXML(IObject * _object, const string & _xmlFile) const
    {
        const auto startLoad = Timer::getTick();
        string relativePath = io::getRelativePath(_xmlFile);

        XMLDoc xmlDoc;
        if (XMLError::XML_SUCCESS == xmlDoc.LoadFile(relativePath.c_str()))
        {
            XMLNode * xmlRoot = xmlDoc.FirstChild();
            if (xmlRoot != nullptr)
            {
                //VG_INFO("[Factory] Load \"%s\"", relativePath.c_str());
                if (SerializeFromXML(_object, xmlDoc))
                {
                    VG_INFO("[Factory] \"%s\" loaded from XML in %.2f ms", relativePath.c_str(), Timer::getEnlapsedTime(startLoad, Timer::getTick()));
                    _object->SetFile(relativePath.c_str());
                    return true;
                }
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::SaveToXML(IObject * _object, const string & _xmlFile) const
    {
        XMLDoc xmlDoc;
        XMLNode * xmlRoot = xmlDoc.NewElement("Root");
        xmlDoc.InsertFirstChild(xmlRoot);

        if (SerializeToXML(_object, xmlDoc))
            if (XMLError::XML_SUCCESS == xmlDoc.SaveFile(_xmlFile.c_str()))
                return true;

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::SaveProperties(core::IObject * _object, BufferType _bufferType)
    {
        auto & buffers = m_buffers[asInteger(_bufferType)];
        auto it = buffers.find(_object->GetUID());

        if (it == buffers.end())
        {
            io::Buffer * buffer = new io::Buffer();
            buffers.insert(std::pair(_object->GetUID(), buffer)).first;
            return serializeObjectToMemory(_object, *buffer, _bufferType);
        }
        else
        {
            VG_SAFE_DELETE(it->second);
            io::Buffer * buffer = new io::Buffer();
            buffers[_object->GetUID()] = buffer;
            return serializeObjectToMemory(_object, *buffer, _bufferType);
        }
        
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::serializeObjectToMemory(const IObject * _object, io::Buffer & _buffer, BufferType _bufferType)
    {
        const char * className = _object->GetClassName();
        const auto * classDesc = GetClassDescriptor(className);

        //VG_DEBUGPRINT("[Factory] Serialize Object \"%s\" (write 0x%016X = %u)\n", _object->GetName().c_str(), &_buffer, _buffer.getWriteOffset());

        for (uint p = 0; p < classDesc->GetPropertyCount(); ++p)
        {
            const auto & prop = classDesc->GetPropertyByIndex(p);
            serializePropertyToMemory(_object, prop, _buffer, _bufferType);
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    void Factory::serializePropertyToMemory(const IObject * _object, const IProperty * _prop, io::Buffer & _buffer, BufferType _bufferType)
    {
        VG_ASSERT(nullptr != _object);

        const auto name = _prop->GetName();
        const auto type = _prop->GetType();
        const auto size = _prop->GetSizeOf();
        const auto offset = _prop->GetOffset();
        const auto flags = _prop->GetFlags();
        const bool isEnumArray = asBool(PropertyFlags::EnumArray & _prop->GetFlags());

        if (asBool(PropertyFlags::NotSaved & flags))
            return;

        //VG_DEBUGPRINT("[Factory] Serialize Property %s->%s (write 0x%016X = %u)\n", _object->GetName().c_str(), _prop->GetName(), &_buffer, _buffer.getWriteOffset());

        switch (type)
        {
            default:
                //VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                VG_WARNING("[Factory] Cannot serialize Property (%s)\"%s\" to memory", asString(type).c_str(), name);
                break;

            // Do nothing
            case PropertyType::Callback:
            case PropertyType::LayoutElement:
                break;

            // POD => Just memcpy
            case PropertyType::Bool:
            case PropertyType::Int8:
            case PropertyType::Int16:
            case PropertyType::Int32:
            case PropertyType::Int64:
            case PropertyType::Uint8:
            case PropertyType::Uint16:
            case PropertyType::Uint32:
            case PropertyType::Uint64:
            case PropertyType::Uint2:
            case PropertyType::Uint3:
            case PropertyType::Uint4:
            case PropertyType::Int2:
            case PropertyType::Int3:
            case PropertyType::Int4:
            case PropertyType::Float:
            case PropertyType::Float2:
            case PropertyType::Float3:
            case PropertyType::Float4:
            case PropertyType::Float4x4:
            case PropertyType::EnumU8:
            case PropertyType::EnumU16:
            case PropertyType::EnumU32:
            case PropertyType::EnumU64:
            case PropertyType::EnumI8:
            case PropertyType::EnumI16:
            case PropertyType::EnumI32:
            case PropertyType::EnumI64:
            case PropertyType::EnumFlagsU8:
            case PropertyType::EnumFlagsU16:
            case PropertyType::EnumFlagsU32:
            case PropertyType::EnumFlagsU64:
            case PropertyType::EnumFlagsI8:
            case PropertyType::EnumFlagsI16:
            case PropertyType::EnumFlagsI32:
            case PropertyType::EnumFlagsI64:
            {
                const void * src = (void *)(uint_ptr(_object) + offset);

                if (isEnumArray)
                {
                    const auto totalSize = _prop->GetEnumCount() * size;
                    VG_VERIFY(_buffer.write(src, totalSize));
                }
                else
                {
                    VG_VERIFY(_buffer.write(src, size));
                }
            }
            break;

            case PropertyType::ObjectHandle:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyToMemory is not implemented for type '%s'", asString(type).c_str());
                const void * src = (void *)(uint_ptr(_object) + offset);
                VG_VERIFY(_buffer.write(src, size));
            }
            break;

            case PropertyType::String:
            {
                const uint count = isEnumArray ? _prop->GetEnumCount() : 1;

                for (uint i = 0; i < count; ++i)
                {
                    const string * s = _prop->GetPropertyString(_object, i);

                    VG_VERIFY(_buffer.write((u32)s->length()));
                    VG_VERIFY(_buffer.write(s->c_str(), s->length()));
                }
            }
            break;

            case PropertyType::BitMask:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyToMemory is not implemented for type '%s'", asString(type).c_str());
                auto * bitMask = _prop->GetPropertyBitMask(_object);
                VG_VERIFY(bitMask->toBuffer(_buffer));
            }
            break;

            case PropertyType::Object:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyToMemory is not implemented for type '%s'", asString(type).c_str());
                IObject * obj = _prop->GetPropertyObject(_object);
                auto uid = obj->GetUID();
                VG_VERIFY(_buffer.write(uid));
                SaveProperties(obj, _bufferType);
            }
            break;

            case PropertyType::Resource:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyToMemory is not implemented for type '%s'", asString(type).c_str());
                IResource * obj = _prop->GetPropertyResource(_object);
                auto uid = obj->GetUID();
                VG_VERIFY(_buffer.write(uid));
                SaveProperties(obj, _bufferType);
            }
            break;

            case PropertyType::ResourceVector:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyToMemory is not implemented for type '%s'", asString(type).c_str());

                auto count = _prop->GetPropertyResourceVectorCount(_object);
                VG_VERIFY(_buffer.write((u64)count));

                if (count > 0)
                {
                    const char * className = _prop->GetPropertyResourceVectorElement(_object, 0)->GetClassName();
                    VG_VERIFY(_buffer.write((u32)strlen(className)));
                    VG_VERIFY(_buffer.write(className, strlen(className)));

                    for (uint i = 0; i < count; ++i)
                    {
                        IResource * obj = _prop->GetPropertyResourceVectorElement(_object, i);
                        auto uid = obj->GetUID();
                        VG_VERIFY(_buffer.write(uid));
                        SaveProperties(obj, _bufferType);
                    }
                }
            }
            break;

            case PropertyType::ObjectPtr:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyToMemory is not implemented for type '%s'", asString(type).c_str());

                IObject ** obj = _prop->GetPropertyObjectPtr(_object);

                if (*obj)
                {
                    auto uid = (*obj)->GetUID();
                    VG_VERIFY(_buffer.write(uid));

                    const char * className = (*obj)->GetClassName();
                    VG_VERIFY(_buffer.write((u32)strlen(className)));
                    VG_VERIFY(_buffer.write(className, strlen(className)));

                    SaveProperties(*obj, _bufferType);
                }
                else
                {
                    VG_VERIFY(_buffer.write((UID)0x0));
                }
            }
            break;

            case PropertyType::ObjectPtrVector:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyToMemory is not implemented for type '%s'", asString(type).c_str());

                auto * vector = _prop->GetPropertyObjectPtrVector(_object);
                VG_VERIFY(_buffer.write((u64)vector->size()));

                for (uint i = 0; i < vector->size(); ++i)
                {
                    auto * obj = (*vector)[i];

                    auto uid = (*vector)[i]->GetUID();
                    VG_VERIFY(_buffer.write(uid));

                    const char * className = obj->GetClassName();
                    VG_VERIFY(_buffer.write((u32)strlen(className)));
                    VG_VERIFY(_buffer.write(className, strlen(className)));

                    SaveProperties(obj, _bufferType);
                }
            }
            break;            
        }
    }

    //--------------------------------------------------------------------------------------
    IObject * Factory::Instanciate(const core::IObject * _object, IObject * _parent)
    {
        const auto className = _object->GetClassName();
        
        IObject * newObj = CreateObject(className, _object->GetName(), _parent);
        VG_ASSERT(nullptr != newObj);
        if (nullptr == newObj)
            return nullptr;

        CopyProperties(_object, newObj);

        return newObj;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::IsA(const char * _class, const char * _other) const
    {
        if (!strcmp(_class, _other))
            return true;

        if (const ClassDesc * classDesc = (const ClassDesc*)GetClassDescriptor(_class, false))
        {
            if (classDesc->parentClassName && strcmp(_class, classDesc->parentClassName))
            {
                if (IsA(classDesc->parentClassName, _other))
                    return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> T Random(T _min, T _max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<T> randomDist(_min, _max);
        return randomDist(gen);
    }

    //--------------------------------------------------------------------------------------
    UID Factory::RegisterUID(IObject * _object)
    {
        lock_guard<mutex> lock(m_uidObjectHashMutex);

        auto uid = _object->GetUID(false);

        if (uid)
        {
            // Object has a valid UID, let's check if it's already in the table
            auto it = m_uidObjectHash.find(uid);
            if (m_uidObjectHash.end() != it)
            {
                // If it's already in the table for the same object then it's OK
                if (it->second == _object)
                {
                    //VG_INFO("[Factory] Already registered UID 0x%08X for object \"%s\"", uid, _object->getName().c_str());
                    return uid;
                }
                else
                {
                    // If it's already in the table but for another object then we must give it a new UID
                    VG_WARNING("[Factory] (%s) \"%s\" has the same UID 0x%08X than (%s) \"%s\"", _object->GetClassDesc()->GetClassName(), _object->GetName().c_str(), uid, it->second->GetClassDesc()->GetClassName(), it->second->GetName().c_str());
                    return getNewUID(_object);
                }
            }
            else
            {
                // Object has UID but is not yet in table so add it
                //VG_INFO("[Factory] Registered UID 0x%08X for object \"%s\"", uid, _object->getName().c_str());
                m_uidObjectHash.insert(std::pair(uid, _object));
                return uid;
            }
        }
        else
        {
            return getNewUID(_object);
        }
    }

    //--------------------------------------------------------------------------------------
    UID Factory::getNewUID(IObject * _object)
    {
        do
        {
            UID uid = Random<u32>(0x00000001, 0xFFFFFFFF);
            auto it = m_uidObjectHash.find(uid);
            if (m_uidObjectHash.end() == it)
            {
                //VG_INFO("[Factory] Registered new UID 0x%08X for object \"%s\"", uid, _object->getName().c_str());
                m_uidObjectHash.insert(std::pair(uid, _object));
                return uid;
            }

        } while (42);
    }

    //--------------------------------------------------------------------------------------
    void Factory::ReleaseUID(IObject * _object, UID & _uid)
    {
        lock_guard<mutex> lock(m_uidObjectHashMutex);

        if (_uid)
        {
            auto it = m_uidObjectHash.find(_uid);
                
            if (m_uidObjectHash.end() != it)
            {
                if (it->second == _object)
                    m_uidObjectHash.erase(it);
            }
            _uid = 0;
        }
    }

    //--------------------------------------------------------------------------------------
    IObject * Factory::FindByUID(UID _uid) const
    {
        lock_guard<mutex> lock(m_uidObjectHashMutex);

        if (_uid)
        {
            auto it = m_uidObjectHash.find(_uid);

            if (m_uidObjectHash.end() != it)
                return it->second;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    const Factory::UIDObjectHash & Factory::GetUIDObjects() const
    {
        return m_uidObjectHash;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::CopyProperties(const core::IObject * _srcObj, core::IObject * _dstObj)
    {
        if (_srcObj == nullptr || _dstObj == nullptr)
            return false;

        // Copy all properties from this to new GameObject
        const auto srcClassName = _srcObj->GetClassName();
        const auto dstClassName = _dstObj->GetClassName();
        VG_ASSERT(dstClassName == srcClassName, "[Factory] Cannot assign object of type \"%s\" to another object of type \"%s\"", srcClassName, dstClassName);

        const auto * classDesc = GetClassDescriptor(srcClassName);

        VG_ASSERT(nullptr != classDesc);
        if (nullptr == classDesc)
            return false;

        const uint propCount = classDesc->GetPropertyCount();
        for (uint i = 0; i < propCount; ++i)
        {
            const IProperty * prop = classDesc->GetPropertyByIndex(i);
            const PropertyFlags propFlags = prop->GetFlags();

            if (asBool(PropertyFlags::NotSaved & propFlags))
                continue;

            CopyProperty(prop, _srcObj, prop, _dstObj);            
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::CanCopyProperty(const core::IProperty * _srcProp, const core::IProperty * _dstProp) const
    {
        const PropertyType srcPropType = _srcProp->GetType();
        const PropertyType dstPropType = _dstProp->GetType();

        if (srcPropType != dstPropType)
            return false;

        const bool srcIsEnumArray = asBool(PropertyFlags::EnumArray & _srcProp->GetFlags());
        const bool dstIsEnumArray = asBool(PropertyFlags::EnumArray & _dstProp->GetFlags());

        if (srcIsEnumArray != dstIsEnumArray)
            return false;

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::CopyProperty(const core::IProperty * _srcProp, const core::IObject * _srcObj, const core::IProperty * _dstProp, core::IObject * _dstObj)
    {
        const PropertyType srcPropType = _srcProp->GetType();
        const PropertyType dstPropType = _dstProp->GetType();

        if (srcPropType != dstPropType)
        {
            VG_WARNING("[Factory] Property \"%s::%s\" have type '%s' but Property \"%s::%s\" have different type '%s'", _srcObj->GetClassName(), _srcProp->GetName(), asString(srcPropType).c_str(), _dstObj->GetClassName(), _dstProp->GetName(), asString(dstPropType).c_str());
            return false;
        }

        const bool srcIsEnumArray = asBool(PropertyFlags::EnumArray & _srcProp->GetFlags());
        const bool dstIsEnumArray = asBool(PropertyFlags::EnumArray & _dstProp->GetFlags());

        if (srcIsEnumArray != dstIsEnumArray)
        {
            VG_WARNING("[Factory] Property \"%s::%s\" have enum count of %u but Property \"%s::%s\" have different enum sizes of %u", _srcObj->GetClassName(), _srcProp->GetName(), _srcProp->GetEnumCount(), _dstObj->GetClassName(), _dstProp->GetName(), _dstProp->GetEnumCount());
            return false;
        }

        if (asBool(_srcProp->GetFlags() & PropertyFlags::NotSaved))
            return false;

        if (!strcmp(_srcProp->GetName(), "m_uid"))
        {
            // In case of DynamicPropertyList the UID must be copied (TODO: use explicit member for different usage?)
            const bool canCopyUID = _srcObj->CanCopyUID();

            if (_srcObj->HasValidUID())
            {
                if (!canCopyUID)
                {
                    if (_srcObj->GetOriginalUID(false)) // equivalent to HasValidOriginalUID
                        _dstObj->SetOriginalUID(_srcObj->GetOriginalUID());
                    else
                        _dstObj->SetOriginalUID(_srcObj->GetUID());

                    _dstObj->RegisterUID();
                }
            }

            if (!canCopyUID)
                return true;
        }
        else if(!strcmp(_srcProp->GetName(), "m_originalUID"))
        {
            return false;
        } 

        switch (srcPropType)
        {
        case PropertyType::Undefined:
            VG_ASSERT_ENUM_NOT_IMPLEMENTED(srcPropType);
            return false;

        case PropertyType::Bool:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyBool(_dstObj) = *_srcProp->GetPropertyBool(_srcObj);
            break;

        case PropertyType::Int8:
        case PropertyType::EnumI8:
        case PropertyType::EnumFlagsI8:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyInt8(_dstObj) = *_srcProp->GetPropertyInt8(_srcObj);
            break;

        case PropertyType::Int16:
        case PropertyType::EnumI16:
        case PropertyType::EnumFlagsI16:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyInt16(_dstObj) = *_srcProp->GetPropertyInt16(_srcObj);
            break;

        case PropertyType::Int32:
        case PropertyType::EnumI32:
        case PropertyType::EnumFlagsI32:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyInt32(_dstObj) = *_srcProp->GetPropertyInt32(_srcObj);
            break;

        case PropertyType::Int64:
        case PropertyType::EnumI64:
        case PropertyType::EnumFlagsI64:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyInt64(_dstObj) = *_srcProp->GetPropertyInt64(_srcObj);
            break;

        case PropertyType::Uint8:
            if (srcIsEnumArray)
                memcpy(_dstProp->GetPropertyUint8(_dstObj), _srcProp->GetPropertyUint8(_srcObj), _srcProp->GetSizeOf() * _srcProp->GetEnumCount());
            else
                *_dstProp->GetPropertyUint8(_dstObj) = *_srcProp->GetPropertyUint8(_srcObj);
            break;

        case PropertyType::EnumU8:
        case PropertyType::EnumFlagsU8:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyUint8(_dstObj) = *_srcProp->GetPropertyUint8(_srcObj);
            break;

        case PropertyType::Uint16:
        case PropertyType::EnumU16:
        case PropertyType::EnumFlagsU16:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyUint16(_dstObj) = *_srcProp->GetPropertyUint16(_srcObj);
            break;

        case PropertyType::Uint32:
        case PropertyType::EnumU32:
        case PropertyType::EnumFlagsU32:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyUint32(_dstObj) = *_srcProp->GetPropertyUint32(_srcObj);
            break;

        case PropertyType::Float:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyFloat(_dstObj) = *_srcProp->GetPropertyFloat(_srcObj);
            break;

        case PropertyType::Float2:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyFloat2(_dstObj) = *_srcProp->GetPropertyFloat2(_srcObj);
            break;

        case PropertyType::Float3:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyFloat3(_dstObj) = *_srcProp->GetPropertyFloat3(_srcObj);
            break;

        case PropertyType::Float4:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyFloat4(_dstObj) = *_srcProp->GetPropertyFloat4(_srcObj);
            break;

        case PropertyType::Float4x4:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyFloat4x4(_dstObj) = *_srcProp->GetPropertyFloat4x4(_srcObj);
            break;

        case PropertyType::Uint64:
        case PropertyType::EnumU64:
        case PropertyType::EnumFlagsU64:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyUint64(_dstObj) = *_srcProp->GetPropertyUint64(_srcObj);
            break;

        case PropertyType::Uint2:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            memcpy(_dstProp->GetPropertyUintN(_dstObj, 2), _srcProp->GetPropertyUintN(_srcObj, 2), sizeof(uint) * 2);
            break;

        case PropertyType::Uint3:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            memcpy(_dstProp->GetPropertyUintN(_dstObj, 3), _srcProp->GetPropertyUintN(_srcObj, 3), sizeof(uint) * 3);
            break;

        case PropertyType::Uint4:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            memcpy(_dstProp->GetPropertyUintN(_dstObj, 4), _srcProp->GetPropertyUintN(_srcObj, 4), sizeof(uint) * 4);
            break;

        case PropertyType::Int2:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            memcpy(_dstProp->GetPropertyIntN(_dstObj, 2), _srcProp->GetPropertyIntN(_srcObj, 2), sizeof(int) * 2);
            break;

        case PropertyType::Int3:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            memcpy(_dstProp->GetPropertyIntN(_dstObj, 3), _srcProp->GetPropertyIntN(_srcObj, 3), sizeof(int) * 3);
            break;

        case PropertyType::Int4:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            memcpy(_dstProp->GetPropertyIntN(_dstObj, 4), _srcProp->GetPropertyIntN(_srcObj, 4), sizeof(int) * 4);
            break;

        case PropertyType::String:
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            *_dstProp->GetPropertyString(_dstObj) = *_srcProp->GetPropertyString(_srcObj);
            break;

        case PropertyType::Object:
        {
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            IObject * srcObj = _srcProp->GetPropertyObject(_srcObj);
            IObject * dstObj = _dstProp->GetPropertyObject(_dstObj);
            CopyProperties(srcObj, dstObj);
        }
        break;

        case PropertyType::ObjectPtr:
        {
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            IObject ** srcObj = _srcProp->GetPropertyObjectPtr(_srcObj);
            IObject ** dstObj = _dstProp->GetPropertyObjectPtr(_dstObj);
            CopyProperties(*srcObj, *dstObj);
        }
        break;

        case PropertyType::ObjectPtrVector:
        {
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            const vector<IObject *> * srcVec = _srcProp->GetPropertyObjectPtrVector(_srcObj);
            const auto count = srcVec->size();

            vector<IObject *> * dstVec = _dstProp->GetPropertyObjectPtrVector(_dstObj);
            VG_ASSERT(dstVec->size() == 0);
            dstVec->reserve(count);

            for (uint i = 0; i < count; ++i)
            {
                IObject * srcChild = (*srcVec)[i];
                IObject * newChild = CreateObject(srcChild->GetClassName(), srcChild->GetName(), _dstObj);
                CopyProperties((IObject *)srcChild, newChild);
                newChild->SetParent(_dstObj);
                dstVec->push_back(newChild);
            }
        }
        break;

        case PropertyType::Resource:
        {
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            IResource * srcRes = _srcProp->GetPropertyResource(_srcObj);
            IResource * dstRes = _dstProp->GetPropertyResource(_dstObj);
            CopyProperties(srcRes, dstRes);

            dstRes->SetParent(_dstObj);
            dstRes->OnResourcePathChanged("", dstRes->GetResourcePath());
        }
        break;

        case PropertyType::ResourceVector:
        {
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            const size_t srcCount = _srcProp->GetPropertyResourceVectorCount(_srcObj);
            const byte * srcData = _srcProp->GetPropertyResourceVectorData(_srcObj);

            if (srcCount > 0)
            {
                const char * elemClassName = _srcProp->GetPropertyResourceVectorElement(_srcObj, 0)->GetClassName();
                const IClassDesc * elemClassDesc = GetClassDescriptor(elemClassName);
                VG_ASSERT(elemClassDesc);
                if (elemClassDesc)
                {
                    uint elementSize;
                    void * data = elemClassDesc->ResizeVector(_dstObj, (uint)_srcProp->GetOffset(), (uint)srcCount, elementSize);

                    for (uint i = 0; i < srcCount; ++i)
                    {
                        IResource * srcRes = _srcProp->GetPropertyResourceVectorElement(_srcObj, i);
                        IResource * dstRes = _dstProp->GetPropertyResourceVectorElement(_dstObj, i);
                        CopyProperties(srcRes, dstRes);

                        dstRes->SetParent(_dstObj);
                        dstRes->OnResourcePathChanged("", dstRes->GetResourcePath());
                    }
                }
            }
        }
        break;

        case PropertyType::BitMask:
        {
            VG_ASSERT(!srcIsEnumArray, "EnumArray CopyProperties serialization not implemented for type '%s'", asString(srcPropType).c_str());
            auto * srcBitMask = _srcProp->GetPropertyBitMask(_srcObj);
            auto * dstBitMask = _dstProp->GetPropertyBitMask(_dstObj);
            *dstBitMask = *srcBitMask;
        }
        break;

        case PropertyType::Callback:
        case PropertyType::LayoutElement:
            // Nothing to do
            break;

        case PropertyType::ObjectHandle:
        {
            auto * srcHandle = _srcProp->GetPropertyObjectHandle(_srcObj);
            auto * dstHandle = _dstProp->GetPropertyObjectHandle(_dstObj);
            *dstHandle = *srcHandle;
        }
        break;

        default:
        case PropertyType::ResourcePtr:
        case PropertyType::ResourcePtrVector:
        case PropertyType::ObjectPtrDictionary:
        case PropertyType::ObjectVector:
            VG_ASSERT_ENUM_NOT_IMPLEMENTED(srcPropType);
            return false;
        }

        _dstObj->OnPropertyChanged(_dstObj, *_dstProp);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::RestoreProperties(core::IObject * _object, BufferType _bufferType)
    {
        auto & buffers = m_buffers[asInteger(_bufferType)];
        auto uid = _object->GetUID(false);
        if (0x0 != uid)
        {
            auto it = buffers.find(uid);
            if (it != buffers.end())
            {
                io::Buffer * buffer = it->second;
                buffer->resetRead();
                bool result = serializeObjectFromMemory(_object, *buffer, _bufferType);

                if (_bufferType == BufferType::InitValue)
                {
                    VG_SAFE_DELETE(it->second);
                    buffers.erase(it);
                }
                return result;
            }
        }
        else
        {
            VG_ERROR("[Factory] Cannot restore properties of (%s)\"%s\" because it has no UID", _object->GetClassName(), _object->GetName().c_str());
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::serializeObjectFromMemory(IObject * _object, io::Buffer & _buffer, BufferType _bufferType)
    {
        const char * className = _object->GetClassName();
        const auto * classDesc = GetClassDescriptor(className);

        //VG_DEBUGPRINT("[Factory] Serialize Object \"%s\" (read 0x%016X = %u)\n", _object->GetName().c_str(), &_buffer, _buffer.getReadOffset());

        for (uint p = 0; p < classDesc->GetPropertyCount(); ++p)
        {
            const auto & prop = classDesc->GetPropertyByIndex(p);
            serializePropertyFromMemory(_object, prop, _buffer, _bufferType);
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    void Factory::serializePropertyFromMemory(IObject * _object, const IProperty * _prop, io::Buffer & _buffer, BufferType _bufferType)
    {
        const char * name = _prop->GetName();
        const auto type = _prop->GetType();
        const auto size = _prop->GetSizeOf();
        const auto offset = _prop->GetOffset();
        const auto flags = _prop->GetFlags();
        const bool isEnumArray = asBool(PropertyFlags::EnumArray & _prop->GetFlags());

        if (asBool(PropertyFlags::NotSaved & flags))
            return;

        //VG_DEBUGPRINT("[Factory] Serialize Property %s->%s (read 0x%016X = %u)\n", _object->GetName().c_str(), _prop->GetName(), &_buffer, _buffer.getReadOffset());

        bool changed = false;

        switch (type)
        {
            default:
                //VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                VG_WARNING("[Factory] Cannot serialize Property (%s)\"%s\" from memory", asString(type).c_str(), name);
                break;

            // Do nothing
            case PropertyType::LayoutElement:
            case PropertyType::Callback:
                break;

            // POD => Just memcpy
            case PropertyType::Bool:
            case PropertyType::Int8:
            case PropertyType::Int16:
            case PropertyType::Int32:
            case PropertyType::Int64:
            case PropertyType::Uint8:
            case PropertyType::Uint16:
            case PropertyType::Uint32:
            case PropertyType::Uint64:
            case PropertyType::Uint2:
            case PropertyType::Uint3:
            case PropertyType::Uint4:
            case PropertyType::Int2:
            case PropertyType::Int3:
            case PropertyType::Int4:
            case PropertyType::Float:
            case PropertyType::Float2:
            case PropertyType::Float3:
            case PropertyType::Float4:
            case PropertyType::Float4x4:
            case PropertyType::EnumU8:
            case PropertyType::EnumU16:
            case PropertyType::EnumU32:
            case PropertyType::EnumU64:
            case PropertyType::EnumI8:
            case PropertyType::EnumI16:
            case PropertyType::EnumI32:
            case PropertyType::EnumI64:
            case PropertyType::EnumFlagsU8:
            case PropertyType::EnumFlagsU16:
            case PropertyType::EnumFlagsU32:
            case PropertyType::EnumFlagsU64:
            case PropertyType::EnumFlagsI8:
            case PropertyType::EnumFlagsI16:
            case PropertyType::EnumFlagsI32:
            case PropertyType::EnumFlagsI64:
            {
                void * dst = (void *)(uint_ptr(_object) + offset);
                if (isEnumArray)
                    VG_VERIFY(_buffer.restore(dst, _prop->GetEnumCount() * size, changed));
                else
                    VG_VERIFY(_buffer.restore(dst, size, changed));
            }
            break;

            case PropertyType::ObjectHandle:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyFromMemory is not implemented for type '%s'", asString(type).c_str());

                void * dst = (void *)(uint_ptr(_object) + offset);
                VG_VERIFY(_buffer.restore(dst, size, changed));
            }
            break;

            case PropertyType::String:
            {
                const uint count = isEnumArray ? _prop->GetEnumCount() : 1;

                for (uint i = 0; i < count; ++i)
                {
                    u32 stringSize = 0;
                    char temp[1024];
                    VG_ASSERT(stringSize < 1024);
                    VG_VERIFY(_buffer.restore(&stringSize, sizeof(u32), changed));
                    VG_VERIFY(_buffer.restore(temp, stringSize, changed));
                    temp[stringSize] = '\0';
                    *_prop->GetPropertyString(_object, i) = temp;
                }           
            }
            break;

            case PropertyType::BitMask:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyFromMemory is not implemented for type '%s'", asString(type).c_str());

                auto * bitMask = _prop->GetPropertyBitMask(_object);
                VG_VERIFY(bitMask->fromBuffer(_buffer));
            }
            break;

            case PropertyType::Object:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyFromMemory is not implemented for type '%s'", asString(type).c_str());

                UID uid = 0;
                VG_VERIFY(_buffer.restore(&uid, sizeof(UID), changed));
                IObject * obj = _prop->GetPropertyObject(_object);
                obj->SetUID(uid);
                RestoreProperties(obj, _bufferType);
            }
            break;

            case PropertyType::Resource:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyFromMemory is not implemented for type '%s'", asString(type).c_str());

                UID uid = 0;
                VG_VERIFY(_buffer.restore(&uid, sizeof(UID), changed));
                IResource * obj = _prop->GetPropertyResource(_object);
                obj->SetUID(uid);
                obj->SetParent(_object);
                RestoreProperties(obj, _bufferType);

                auto path = obj->GetResourcePath();
                if (!path.empty() && nullptr == obj->GetObject())
                    obj->OnResourcePathChanged("", path);
            }
            break;

            case PropertyType::ResourceVector:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyFromMemory is not implemented for type '%s'", asString(type).c_str());

                u64 count = 0;
                VG_VERIFY(_buffer.read(&count));

                if (count > 0)
                {
                    u32 stringSize = 0;
                    char className[1024];
                    VG_ASSERT(stringSize < 1024);
                    VG_VERIFY(_buffer.restore(&stringSize, sizeof(u32), changed));
                    VG_VERIFY(_buffer.restore(className, stringSize, changed));
                    className[stringSize] = '\0';

                    const IClassDesc * elemClassDesc = GetClassDescriptor(className);
                    VG_ASSERT(elemClassDesc);
                    if (count != _prop->GetPropertyResourceVectorCount(_object))
                    {
                        uint elementSize;
                        elemClassDesc->ResizeVector(_object, (uint)_prop->GetOffset(), (uint)count, elementSize);
                    }

                    for (uint i = 0; i < count; ++i)
                    {
                        UID uid = 0;
                        VG_VERIFY(_buffer.restore(&uid, sizeof(UID), changed));

                        IResource * obj = _prop->GetPropertyResourceVectorElement(_object, i);
                        obj->SetUID(uid);
                        obj->SetParent(_object);
                        RestoreProperties(obj, _bufferType);

                        auto path = obj->GetResourcePath();
                        if (!path.empty() && nullptr == obj->GetObject())
                            obj->OnResourcePathChanged("", path);
                    }
                }
            }
            break;

            case PropertyType::ObjectPtr:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyFromMemory is not implemented for type '%s'", asString(type).c_str());

                UID uid = 0;
                VG_VERIFY(_buffer.restore(&uid, sizeof(UID), changed));

                if (0 != uid)
                {
                    u32 stringSize = 0;
                    char className[1024];
                    VG_ASSERT(stringSize < 1024);
                    VG_VERIFY(_buffer.restore(&stringSize, sizeof(u32), changed));
                    VG_VERIFY(_buffer.restore(className, stringSize, changed));
                    className[stringSize] = '\0';

                    auto ** obj = _prop->GetPropertyObjectPtr(_object);

                    if (nullptr != *obj)
                    {
                        RestoreProperties(*obj, _bufferType);
                    }
                    else
                    {
                        IObject * newObj = CreateObject(className, "", _object);
                        newObj->SetUID(uid);
                        RestoreProperties(newObj, _bufferType);
                        newObj->OnLoad();
                        *obj = newObj;
                    }
                }
            }
            break;

            case PropertyType::ObjectPtrVector:
            {
                VG_ASSERT(!isEnumArray, "EnumArray serializePropertyFromMemory is not implemented for type '%s'", asString(type).c_str());

                u64 count = 0;
                VG_VERIFY(_buffer.restore(&count, sizeof(u64), changed));

                auto * vector = _prop->GetPropertyObjectPtrVector(_object);
                for (uint i = 0; i < count; ++i)
                {
                    UID uid = 0;
                    VG_VERIFY(_buffer.restore(&uid, sizeof(UID), changed));

                    u32 stringSize = 0;
                    char className[1024];
                    VG_ASSERT(stringSize < 1024);
                    VG_VERIFY(_buffer.restore(&stringSize, sizeof(u32), changed));
                    VG_VERIFY(_buffer.restore(className, stringSize, changed));
                    className[stringSize] = '\0';

                    if (i < vector->size())
                    {
                        auto * obj = (*vector)[i];
                        RestoreProperties((*vector)[i], _bufferType);
                    }
                    else
                    {
                        //VG_WARNING("[Factory] Need to create an Object of type \"%s\"", className);
                        IObject * newObj = CreateObject(className, "", _object);
                        newObj->SetUID(uid);
                        RestoreProperties(newObj, _bufferType);
                        newObj->OnLoad();
                        (*vector).push_back(newObj);
                    }
                }
            }
            break;
        }
    }

    //--------------------------------------------------------------------------------------
    core::uint getComponentCount(PropertyType _type)
    {
        switch (_type)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_type);
                return 1;

            case PropertyType::Float2:
            case PropertyType::Uint2:
            case PropertyType::Int2:
                return 2;

            case PropertyType::Float3:
            case PropertyType::Uint3:
            case PropertyType::Int3:
                return 3;

            case PropertyType::Float4:
            case PropertyType::Uint4:
            case PropertyType::Int4:
                return 4;
        }
    }

    //--------------------------------------------------------------------------------------
    const IClassDesc * Factory::GetClassDescriptor(const char * _className, bool _mustExist) const
    {
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            const auto & classDesc = m_classes[i];
            if (!strcmp(classDesc.name, _className))
                return &classDesc;
        }

        VG_ASSERT(!_mustExist, "class \"%s\" has no class descriptor", _className);
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    const vector<IClassDesc *> Factory::GetClassDescriptors(ClassDescFlags _required, ClassDescFlags _excluded) const
    {
        vector<IClassDesc *> classes;
        for (uint i = 0; i < m_classes.size(); ++i)
        {
            auto & clas = m_classes[i];
            if ((asBool(clas.flags & _required) || (u64)_required == -1) && !asBool(clas.flags & _excluded))
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
            const std::string & str = asString(enumVal);
            if (!strcmp(str.data(), _value))
                return enumVal;
        }
        return _default;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::SerializeFromXML(IObject* _object, XMLDoc & _xmlDoc) const
    {
        auto * parent = _xmlDoc.RootElement();
        const XMLElement * xmlObject = parent->FirstChildElement("Object");
        bool result = SerializeFromXML(_object, xmlObject);
        return result;
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
    bool Factory::SerializeFromXML(IObject * _object, const XMLElem * xmlObject) const
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
                const char * className = fixDeprecatedClassName(xmlClassAttr->Value());
                const auto * classDesc = GetClassDescriptor(className);
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
                            auto type = enumGetValue<PropertyType>(typeName);

                            if (PropertyType::Undefined == type)
                            {
                                auto it = m_oldTypeNames.find(typeName);
                                if (m_oldTypeNames.end() != it)
                                {
                                    VG_WARNING("[Factory] Deprecated type \"%s\" for property \"%s\" replaced by \"%s\" in \"%s\"", typeName, name, asString(it->second).c_str(), _object->GetShortName().c_str());
                                    type = it->second;
                                }
                            }

                            const auto * prop = classDesc->GetPropertyByName(name);

                            if (nullptr == prop)
                            {
                                auto it = m_oldPropertyNames.find(name);
                                if (m_oldPropertyNames.end() != it)
                                {
                                    VG_WARNING("[Factory] Deprecated name for property \"%s\" replaced by \"%s\" in \"%s\"", name, it->second.c_str(), _object->GetShortName().c_str());
                                    name = it->second.c_str();
                                    prop = classDesc->GetPropertyByName(name);
                                }
                                else
                                {
                                    VG_WARNING("[Factory] Class \"%s\" has no property \"%s\" of type '%s' in \"%s\"", className, name, typeName, _object->GetShortName().c_str());
                                }
                            }
                            
                            if (nullptr == prop || asBool(PropertyFlags::NotSaved & prop->GetFlags()))
                            {

                            }
                            else
                            {
                                const bool isEnumArray = asBool(PropertyFlags::EnumArray & prop->GetFlags());

                                if (prop->GetType() == type)
                                {
                                    const auto offset = prop->GetOffset();

                                    switch (type)
                                    {
                                        default:
                                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                                            break;

                                        case PropertyType::LayoutElement:
                                            break;

                                        case PropertyType::BitMask:
                                        {
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());

                                            const XMLAttribute * xmlBitCount = xmlPropElem->FindAttribute("BitCount");
                                            const XMLAttribute * xmlValue = xmlPropElem->FindAttribute("Value");
                                            if (nullptr != xmlValue && nullptr != xmlBitCount)
                                            {
                                                uint bitCount = xmlBitCount->UnsignedValue();

                                                BitMask * pBitMask = prop->GetPropertyBitMask(_object); 
                                                string sVal = xmlValue->Value();

                                                pBitMask->fromString(sVal, bitCount);
                                            }
                                        }
                                        break;

                                        case PropertyType::Float:
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

                                        case PropertyType::Float4x4:
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

                                        case PropertyType::Object:
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
                                                    const auto * classDescRef = GetClassDescriptor(classNameRef);
                                                    if (nullptr != classDescRef)
                                                    {
                                                        // TODO: clear contents?
                                                        VG_VERIFY(SerializeFromXML(pObjectRef, xmlObjectRef));
                                                        pObjectRef->SetParent(_object);
                                                    }
                                                }
                                            }
                                        }
                                        break;

                                        case PropertyType::Resource:
                                        case PropertyType::ResourcePtr:
                                        {
                                            const bool ref = type == PropertyType::ResourcePtr;

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
                                                        classDescRef = GetClassDescriptor(classNameRef);
                                                        if (nullptr != classDescRef)
                                                        {
                                                            if (SerializeFromXML(_resource, xmlObjectRef))
                                                            {
                                                                _resource->SetParent(_object);
                                                                _resource->OnResourcePathChanged("", _resource->GetResourcePath());
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
                                                            for (uint i = 0; i < prop->GetEnumCount(); ++i)
                                                            {
                                                                if (!strcmp(xmlValueName->Value(), prop->GetEnumName(i).c_str()))
                                                                {
                                                                    IResource * pResource = ref ? *prop->GetPropertyResourcePtr(_object, i) : prop->GetPropertyResource(_object, i);
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
                                                IResource * pResource = ref ? *prop->GetPropertyResourcePtr(_object) : prop->GetPropertyResource(_object);
                                                serializeResourceFromXML(pResource, xmlPropElem);
                                            }
                                        }
                                        break;

                                        case PropertyType::ResourcePtrVector:
                                        {
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            const XMLElement * xmlObjectRef = xmlPropElem->FirstChildElement("Object");
                                            if (nullptr != xmlObjectRef)
                                            {
                                                auto * vector = prop->GetPropertyResourcePtrVector(_object);
                                                const XMLElement * xmlObjectRef = xmlPropElem->FirstChildElement("Object");

                                                do
                                                {
                                                    if (nullptr != xmlObjectRef)
                                                    {
                                                        const XMLAttribute * xmlClassAttrRef = xmlObjectRef->FindAttribute("class");
                                                        if (nullptr != xmlClassAttrRef)
                                                        {
                                                            const char * classNameRef = xmlClassAttrRef->Value();
                                                            const auto * classDescRef = GetClassDescriptor(classNameRef);
                                                            if (nullptr != classDescRef)
                                                            {
                                                                IResource * resource = (IResource*)CreateObject(classNameRef, "", _object);
                                                                if (SerializeFromXML(resource, xmlObjectRef))
                                                                {
                                                                    resource->SetParent(_object);
                                                                    resource->OnResourcePathChanged("", resource->GetResourcePath());
                                                                    vector->push_back(resource);
                                                                }
                                                                else
                                                                    VG_SAFE_DELETE(resource);
                                                            }
                                                        }
                                                    }

                                                    xmlObjectRef = xmlObjectRef->NextSiblingElement("Object");

                                                } while (xmlObjectRef != nullptr);
                                            }
                                        }
                                        break;

                                        case PropertyType::ResourceVector:
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
                                                        const auto * classDescRef = GetClassDescriptor(classNameRef);
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
                                                            const auto * classDescRef = GetClassDescriptor(classNameRef);
                                                            if (nullptr != classDescRef)
                                                            {
                                                                IResource * pResource = (IResource *)(uint_ptr(data) + index * elemSize);
                                                                if (SerializeFromXML(pResource, xmlObjectRef))
                                                                {
                                                                    pResource->SetParent(_object);
                                                                    pResource->OnResourcePathChanged("", pResource->GetResourcePath());
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

                                        case PropertyType::ObjectPtr:
                                        {
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            IObject* pObjectRef = nullptr;
                                            const XMLElement* xmlObjectRef = xmlPropElem->FirstChildElement("Object");
                                            if (nullptr != xmlObjectRef)
                                            {
                                                const XMLAttribute* xmlClassAttrRef = xmlObjectRef->FindAttribute("class");
                                                if (nullptr != xmlClassAttrRef)
                                                {
                                                    const char* classNameRef = fixDeprecatedClassName(xmlClassAttrRef->Value());
                                                    const auto* classDescRef = GetClassDescriptor(classNameRef);
                                                    if (nullptr != classDescRef)
                                                    {
                                                        pObjectRef = CreateObject(classNameRef, "", _object);
                                                        if (!SerializeFromXML(pObjectRef, xmlObjectRef))
                                                            VG_SAFE_DELETE(pObjectRef);
                                                    }
                                                }
                                            }
                                        
                                            IObject** ppObjectRef = (IObject**)(uint_ptr(_object) + offset);
                                            VG_SAFE_RELEASE((*ppObjectRef));
                                            *ppObjectRef = pObjectRef;
                                        }
                                        break;

                                        case PropertyType::ObjectPtrVector:
                                        {
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            const XMLElement* xmlObjectRef = xmlPropElem->FirstChildElement("Object");
                                            if (nullptr != xmlObjectRef)
                                            {
                                                auto * vector = prop->GetPropertyObjectPtrVector(_object);
                                                const XMLElement* xmlObjectRef = xmlPropElem->FirstChildElement("Object");
                                        
                                                do
                                                {
                                                    IObject* pObjectRef = nullptr;
                                        
                                                    if (nullptr != xmlObjectRef)
                                                    {
                                                        const XMLAttribute* xmlClassAttrRef = xmlObjectRef->FindAttribute("class");
                                                        if (nullptr != xmlClassAttrRef)
                                                        {
                                                            const char* classNameRef = fixDeprecatedClassName(xmlClassAttrRef->Value());
                                                            const auto* classDescRef = GetClassDescriptor(classNameRef);
                                                            if (nullptr != classDescRef)
                                                            {
                                                                pObjectRef = CreateObject(classNameRef, "", _object);
                                                                if (SerializeFromXML(pObjectRef, xmlObjectRef))
                                                                {
                                                                    vector->push_back(pObjectRef);
                                                                }
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

                                        case PropertyType::EnumFlagsU8:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumFlagsPropertyFromXML<u8>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::EnumFlagsU16:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumFlagsPropertyFromXML<u16>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::EnumFlagsU32:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumFlagsPropertyFromXML<u32>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::EnumFlagsU64:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumFlagsPropertyFromXML<u64>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::String:
                                        {
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
                                                            for (uint i = 0; i < prop->GetEnumCount(); ++i)
                                                            {
                                                                if (!strcmp(xmlValueName->Value(), prop->GetEnumName(i).c_str()))
                                                                {
                                                                    if (const XMLAttribute * xmlValue = xmlPropElemValue->FindAttribute("value"))
                                                                    {
                                                                        string * pString = prop->GetPropertyString(_object, i);
                                                                        *pString = xmlValue->Value();
                                                                    }
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
                                                const XMLAttribute * xmlValue = xmlPropElem->FindAttribute("value");
                                                if (nullptr != xmlValue)
                                                {
                                                    string * pString = prop->GetPropertyString(_object);
                                                    *pString = xmlValue->Value();
                                                }
                                            }
                                        }
                                        break;

                                        case PropertyType::Bool:
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

                                        case PropertyType::Uint2:
                                        case PropertyType::Uint3:
                                        case PropertyType::Uint4:
                                        {
                                            auto serializeUintNFromXML = [](u32 * _value, const XMLElement * _xmlElement, uint _componentCount)
                                            {
                                                u32 * pUint = (u32 *)_value;
                                                const char * values[] = { "x", "y", "z", "w" };
                                                for (uint i = 0; i < _componentCount; ++i)
                                                {
                                                    const XMLAttribute * xmlValue = _xmlElement->FindAttribute(values[i]);
                                                    if (nullptr != xmlValue)
                                                        pUint[i] = xmlValue->UnsignedValue();
                                                }
                                            };

                                            const auto componentCount = getComponentCount(type);

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
                                                            for (uint i = 0; i < prop->GetEnumCount(); ++i)
                                                            {
                                                                if (!strcmp(xmlValueName->Value(), prop->GetEnumName(i).c_str()))
                                                                {
                                                                    u32 * pUint = prop->GetPropertyUintN(_object, componentCount, i);
                                                                    serializeUintNFromXML(pUint, xmlPropElemValue, componentCount);
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
                                                u32 * pUint = prop->GetPropertyUintN(_object, componentCount);
                                                serializeUintNFromXML(pUint, xmlPropElem, componentCount);
                                            }
                                        }
                                        break;

                                        case PropertyType::Int2:
                                        case PropertyType::Int3:
                                        case PropertyType::Int4:
                                        {                     
                                            auto serializeIntNFromXML = [](i32 * _value, const XMLElement * _xmlElement, uint _componentCount)
                                            {
                                                i32 * pInt = (i32 *)_value;
                                                const char * values[] = { "x", "y", "z", "w" };
                                                for (uint i = 0; i < _componentCount; ++i)
                                                {
                                                    const XMLAttribute * xmlValue = _xmlElement->FindAttribute(values[i]);
                                                    if (nullptr != xmlValue)
                                                        pInt[i] = xmlValue->IntValue();
                                                }
                                            };

                                            const auto componentCount = getComponentCount(type);

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
                                                            for (uint i = 0; i < prop->GetEnumCount(); ++i)
                                                            {
                                                                if (!strcmp(xmlValueName->Value(), prop->GetEnumName(i).c_str()))
                                                                {
                                                                    i32 * pInt = prop->GetPropertyIntN(_object, componentCount, i);
                                                                    serializeIntNFromXML(pInt, xmlPropElemValue, componentCount);
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
                                                i32 * pInt = prop->GetPropertyIntN(_object, componentCount);
                                                serializeIntNFromXML(pInt, xmlPropElem, componentCount);
                                            }
                                        }
                                        break;

                                        case PropertyType::Float2:
                                        case PropertyType::Float3:
                                        case PropertyType::Float4:
                                        {
                                            auto serializeFloatNFromXML = [](float * _value, const XMLElement * _xmlElement, uint _componentCount)
                                            {
                                                float * pFloat = (float *)_value;
                                                const char * values[] = { "x", "y", "z", "w" };
                                                for (uint i = 0; i < _componentCount; ++i)
                                                {
                                                    const XMLAttribute * xmlValue = _xmlElement->FindAttribute(values[i]);
                                                    if (nullptr != xmlValue)
                                                        pFloat[i] = xmlValue->FloatValue();
                                                }
                                            };

                                            const auto componentCount = getComponentCount(type);

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
                                                            for (uint i = 0; i < prop->GetEnumCount(); ++i)
                                                            {
                                                                if (!strcmp(xmlValueName->Value(), prop->GetEnumName(i).c_str()))
                                                                {
                                                                    float * pFloat = prop->GetPropertyFloatN(_object, componentCount, i);
                                                                    serializeFloatNFromXML(pFloat, xmlPropElemValue, componentCount);
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
                                                float * pFloat = prop->GetPropertyFloatN(_object, componentCount);
                                                serializeFloatNFromXML(pFloat, xmlPropElem, componentCount);
                                            }
                                        }
                                        break;

                                        case PropertyType::EnumU8:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumPropertyFromXML<u8>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::EnumU16:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumPropertyFromXML<u16>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::EnumU32:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeEnumPropertyFromXML<u32>(_object, prop, xmlPropElem);
                                        break;

                                        case PropertyType::Uint8:
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
                                                            for (uint i = 0; i < prop->GetEnumCount(); ++i)
                                                            {
                                                                if (!strcmp(xmlValueName->Value(), prop->GetEnumName(i).c_str()))
                                                                {
                                                                    serializeIntegerPropertyFromXML<u8>(_object, prop, xmlPropElemValue, i);
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
                                                serializeIntegerPropertyFromXML<u8>(_object, prop, xmlPropElem);
                                            }
                                        break;

                                        case PropertyType::Uint16:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<u16>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::Uint32:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<u32>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::ObjectHandle:
                                        {
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<u32>(_object, prop, xmlPropElem);
                                            const XMLAttribute * xmlValue = xmlPropElem->FindAttribute("value");
                                            if (nullptr != xmlValue)
                                            {
                                                ObjectHandle * objHandle = prop->GetPropertyObjectHandle(_object);
                                                objHandle->setUID(xmlValue->UnsignedValue());
                                            }
                                        }
                                        break;

                                        case PropertyType::Uint64:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<u64>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::Int8:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<i8>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::Int16:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<i16>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::Int32:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<i32>(_object, prop, xmlPropElem);
                                            break;

                                        case PropertyType::Int64:
                                            VG_ASSERT(!isEnumArray, "EnumArray serialization from XML not implemented for type '%s'", asString(type).c_str());
                                            serializeIntegerPropertyFromXML<i64>(_object, prop, xmlPropElem);
                                            break;
                                    }
                                }
                                else
                                {
                                    VG_WARNING("[Factory] Serialized Object type \"%s\" for Property \"%s\" from class \"%s\" does not match type \"%s\" declared in ClassDesc\n", typeName, name, className, asString(prop->GetType()).c_str());
                                }
                            }
                        }

                        xmlPropElem = xmlPropElem->NextSiblingElement("Property");
                    }
                }
            }
        }
        _object->OnLoad();
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Factory::SerializeToXML(IObject * _object, XMLDoc & _xmlDoc, XMLElem * _parent) const
    {
        auto * parent = nullptr != _parent ? _parent : _xmlDoc.RootElement();

        const char * className = _object->GetClassName();

        if (auto instance = dynamic_cast<const IInstance *>(_object))
        {
            if (asBool(ObjectFlags::NotSerialized & instance->GetObjectFlags()))
                return false;
        }

        _object->OnSave();

        XMLElement * xmlElement = _xmlDoc.NewElement("Object");
        xmlElement->SetAttribute("class", className);

        const auto * classDesc = GetClassDescriptor(className);

        for (uint p = 0; p < classDesc->GetPropertyCount(); ++p)
        {
            const auto & prop = classDesc->GetPropertyByIndex(p);

            const auto name = prop->GetName();
            const auto type = prop->GetType();
            const auto offset = prop->GetOffset();
            const auto flags = prop->GetFlags();

            if (asBool(PropertyFlags::NotSaved & flags) || type == PropertyType::LayoutElement)
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

            const bool isEnumArray = asBool(PropertyFlags::EnumArray & prop->GetFlags());

            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    skipAttribute = true;
                    break;

                case PropertyType::LayoutElement:
                    break;

                case PropertyType::Callback:
                {
                    skipAttribute = true;
                }
                break;

                case PropertyType::BitMask:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());

                    const BitMask * pBitMask = prop->GetPropertyBitMask(_object);
                    const uint bitCount = pBitMask->getBitCount();

                    xmlPropElem->SetAttribute("BitCount", bitCount);
                    xmlPropElem->SetAttribute("Value", pBitMask->toString(true).c_str());
                }
                break;

                case PropertyType::Int8:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeIntegerPropertyToXML<i8>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::Int16:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeIntegerPropertyToXML<i16>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::Int32:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeIntegerPropertyToXML<i32>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::Int64:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeIntegerPropertyToXML<i64>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::Uint8:
                    if (isEnumArray)
                    {
                        const uint count = prop->GetEnumCount();
                        for (uint i = 0; i < count; ++i)
                        {
                            XMLElement * xmlPropElemChild = _xmlDoc.NewElement("Value");
                            {
                                auto enumValueName = prop->GetEnumName(i);
                                xmlPropElemChild->SetAttribute("name", enumValueName.c_str());
                                serializeIntegerPropertyToXML<u8>(_object, prop, xmlPropElemChild, i);
                            }
                            xmlPropElem->InsertEndChild(xmlPropElemChild);
                        }
                    }
                    else
                    {
                        serializeIntegerPropertyToXML<u8>(_object, prop, xmlPropElem);
                    }
                    break;

                case PropertyType::Uint16:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeIntegerPropertyToXML<u16>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::Uint32:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeIntegerPropertyToXML<u32>(_object, prop, xmlPropElem);
                break;

                case PropertyType::Uint64:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeIntegerPropertyToXML<u64>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::ObjectHandle:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    const ObjectHandle * objHandle = prop->GetPropertyObjectHandle(_object);
                    xmlPropElem->SetAttribute("value", objHandle->getUID());
                }
                break;

                case PropertyType::Resource:
                case PropertyType::ResourcePtr:
                {
                    const bool ref = type == PropertyType::ResourcePtr;

                    if (isEnumArray)
                    {
                        const uint count = prop->GetEnumCount();
                        for (uint i = 0; i < count; ++i)
                        {
                            XMLElement * xmlPropElemChild = _xmlDoc.NewElement("Value");
                            {
                                auto enumValueName = prop->GetEnumName(i);
                                xmlPropElemChild->SetAttribute("name", enumValueName.c_str());
                                IObject * pResource = ref ? *prop->GetPropertyResourcePtr(_object, i) : prop->GetPropertyResource(_object, i);
                                SerializeToXML(pResource, _xmlDoc, xmlPropElemChild);
                            }
                            xmlPropElem->InsertEndChild(xmlPropElemChild);
                        }
                    }
                    else
                    {
                        IObject * pResource = ref ? *prop->GetPropertyResourcePtr(_object) : prop->GetPropertyResource(_object);
                        SerializeToXML(pResource, _xmlDoc, xmlPropElem);
                    }
                }
                break;

                case PropertyType::ResourceVector:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    const uint sizeOf = prop->GetSizeOf();
                    const size_t count = prop->GetPropertyResourceVectorCount(_object);
                    const byte * data = prop->GetPropertyResourceVectorData(_object);

                    for (uint i = 0; i < count; ++i)
                    {
                        IObject * pObject = (IObject *)(data + sizeOf * i);
                        SerializeToXML(pObject, _xmlDoc, xmlPropElem);
                    }
                }
                break;

                case PropertyType::Object:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    IObject * pObject = prop->GetPropertyObject(_object);
                    SerializeToXML(pObject, _xmlDoc, xmlPropElem);
                }
                break;

                case PropertyType::ObjectPtr:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    IObject * pObject = *prop->GetPropertyObjectPtr(_object);
                    if (nullptr != pObject)
                        SerializeToXML(pObject, _xmlDoc, xmlPropElem);
                }
                break;

                case PropertyType::ObjectVector:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    const uint sizeOf = prop->GetSizeOf();
                    const size_t count = prop->GetPropertyObjectVectorCount(_object);
                    const byte * data = prop->GetPropertyObjectVectorData(_object);

                    for (uint i = 0; i < count; ++i)
                    {
                        IObject * pObject = (IObject *)(data + sizeOf * i);
                        SerializeToXML(pObject, _xmlDoc, xmlPropElem);
                    }
                }
                break;

                case PropertyType::ResourcePtrVector:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    auto * vector = prop->GetPropertyResourcePtrVector(_object);
                    const auto count = vector->size();
                    for (auto i = 0; i < count; ++i)
                        SerializeToXML((*vector)[i], _xmlDoc, xmlPropElem);
                }
                break;

                case PropertyType::ObjectPtrVector:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    auto * vector = prop->GetPropertyObjectPtrVector(_object);
                    const auto count = vector->size();
                    for (auto i = 0; i < count; ++i)
                        SerializeToXML((*vector)[i], _xmlDoc, xmlPropElem);
                }
                break;

                case PropertyType::String:
                {
                    if (isEnumArray)
                    {
                        const uint count = prop->GetEnumCount();
                        for (uint i = 0; i < count; ++i)
                        {
                            const string * pString = prop->GetPropertyString(_object, i);

                            if (!pString->empty())
                            {
                                XMLElement * xmlPropElemChild = _xmlDoc.NewElement("Value");
                                {
                                    auto enumValueName = prop->GetEnumName(i);
                                    xmlPropElemChild->SetAttribute("name", enumValueName.c_str());

                                    xmlPropElemChild->SetAttribute("value", pString->c_str());
                                }
                                xmlPropElem->InsertEndChild(xmlPropElemChild);
                            }
                        }
                    }
                    else
                    {
                        const string * pString = prop->GetPropertyString(_object);
                        xmlPropElem->SetAttribute("value", pString->c_str());
                    }
                }
                break;

                case PropertyType::Bool:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    const bool * pBool = prop->GetPropertyBool(_object);
                    xmlPropElem->SetAttribute("value", *pBool);
                }
                break;

                case PropertyType::Float:
                {
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    const float * pFloat = prop->GetPropertyFloat(_object);
                    xmlPropElem->SetAttribute("value", *pFloat);
                }
                break;

                case PropertyType::Uint2:
                case PropertyType::Uint3:
                case PropertyType::Uint4:
                {
                    auto serializeUintNToXML = [](XMLElement * _xmlElement, const uint * pUint, uint _componentCount)
                    {
                        _xmlElement->SetAttribute("x", pUint[0]);
                        if (_componentCount > 1)
                        {
                            _xmlElement->SetAttribute("y", pUint[1]);
                            if (_componentCount > 2)
                            {
                                _xmlElement->SetAttribute("z", pUint[2]);

                                if (_componentCount > 3)
                                {
                                    _xmlElement->SetAttribute("w", pUint[3]);
                                }
                            }
                        }
                    };

                    const uint componentCount = getComponentCount(type);

                    if (isEnumArray)
                    {
                        const uint count = prop->GetEnumCount();
                        for (uint i = 0; i < count; ++i)
                        {
                            XMLElement * xmlPropElemChild = _xmlDoc.NewElement("Value");
                            {
                                auto enumValueName = prop->GetEnumName(i);
                                xmlPropElemChild->SetAttribute("name", enumValueName.c_str());
                                const uint * value = prop->GetPropertyUintN(_object, componentCount, i);
                                serializeUintNToXML(xmlPropElemChild, value, componentCount);
                            }
                            xmlPropElem->InsertEndChild(xmlPropElemChild);
                        }
                    }
                    else
                    {
                        const u32 * value = prop->GetPropertyUintN(_object, componentCount);
                        serializeUintNToXML(xmlPropElem, value, componentCount);
                    }
                }
                break;

                case PropertyType::Int2:
                case PropertyType::Int3:
                case PropertyType::Int4:
                {
                    auto serializeIntNToXML = [](XMLElement * _xmlElement, const int * pInt, uint _componentCount)
                        {
                            _xmlElement->SetAttribute("x", pInt[0]);
                            if (_componentCount > 1)
                            {
                                _xmlElement->SetAttribute("y", pInt[1]);
                                if (_componentCount > 2)
                                {
                                    _xmlElement->SetAttribute("z", pInt[2]);

                                    if (_componentCount > 3)
                                    {
                                        _xmlElement->SetAttribute("w", pInt[3]);
                                    }
                                }
                            }
                        };

                    const uint componentCount = getComponentCount(type);

                    if (isEnumArray)
                    {
                        const uint count = prop->GetEnumCount();
                        for (uint i = 0; i < count; ++i)
                        {
                            XMLElement * xmlPropElemChild = _xmlDoc.NewElement("Value");
                            {
                                auto enumValueName = prop->GetEnumName(i);
                                xmlPropElemChild->SetAttribute("name", enumValueName.c_str());
                                const i32 * value = prop->GetPropertyIntN(_object, componentCount, i);
                                serializeIntNToXML(xmlPropElemChild, value, componentCount);
                            }
                            xmlPropElem->InsertEndChild(xmlPropElemChild);
                        }
                    }
                    else
                    {
                        const i32 * value = prop->GetPropertyIntN(_object, componentCount);
                        serializeIntNToXML(xmlPropElem, value, componentCount);
                    }
                }
                break;

                case PropertyType::Float2:
                case PropertyType::Float3:
                case PropertyType::Float4:
                {
                    auto serializeFloatNToXML = [](XMLElement * _xmlElement, const float * pFloat, uint _componentCount)
                    {
                        _xmlElement->SetAttribute("x", pFloat[0]);
                        if (_componentCount > 1)
                        {
                            _xmlElement->SetAttribute("y", pFloat[1]);
                            if (_componentCount > 2)
                            {
                                _xmlElement->SetAttribute("z", pFloat[2]);

                                if (_componentCount > 3)
                                {
                                    _xmlElement->SetAttribute("w", pFloat[3]);
                                }
                            }
                        }
                    };

                    const uint componentCount = getComponentCount(type);

                    if (isEnumArray)
                    {
                        const uint count = prop->GetEnumCount();
                        for (uint i = 0; i < count; ++i)
                        {
                            XMLElement * xmlPropElemChild = _xmlDoc.NewElement("Value");
                            {
                                auto enumValueName = prop->GetEnumName(i);
                                xmlPropElemChild->SetAttribute("name", enumValueName.c_str());
                                const float * value = prop->GetPropertyFloatN(_object, componentCount, i);
                                serializeFloatNToXML(xmlPropElemChild, value, componentCount);
                            }
                            xmlPropElem->InsertEndChild(xmlPropElemChild);
                        }
                    }
                    else
                    {
                        const float * value = prop->GetPropertyFloatN(_object, componentCount);
                        serializeFloatNToXML(xmlPropElem, value, componentCount);
                    }
                }
                break;

                case PropertyType::Float4x4:
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

                case PropertyType::EnumFlagsU8:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumFlagsPropertyToXML<u8>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::EnumFlagsU16:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumFlagsPropertyToXML<u16>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::EnumFlagsU32:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumFlagsPropertyToXML<u32>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::EnumFlagsU64:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumFlagsPropertyToXML<u64>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::EnumU8:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumPropertyToXML<u8>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::EnumU16:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumPropertyToXML<u16>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::EnumU32:
                    VG_ASSERT(!isEnumArray, "EnumArray serialization to XML not implemented for type '%s'", asString(type).c_str());
                    serializeEnumPropertyToXML<u32>(_object, prop, xmlPropElem);
                    break;

                case PropertyType::EnumU64:
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

    template <typename T> T GetValue(const XMLAttribute * xmlValue);

    template <> i8 GetValue<i8>(const XMLAttribute * xmlValue)
    {
        return xmlValue->IntValue(); // CharValue
    }

    template <> u8 GetValue<u8>(const XMLAttribute * xmlValue)
    {
        return xmlValue->UnsignedValue(); // UnsignedCharValue
    }

    template <> i16 GetValue<i16>(const XMLAttribute * xmlValue)
    {
        return xmlValue->IntValue(); // ShortValue
    }

    template <> u16 GetValue<u16>(const XMLAttribute * xmlValue)
    {
        return xmlValue->UnsignedValue(); // UnsignedShortValue
    }

    template <> u32 GetValue<u32>(const XMLAttribute * xmlValue)
    {
        return xmlValue->UnsignedValue();
    }

    template <> i32 GetValue<i32>(const XMLAttribute * xmlValue)
    {
        return xmlValue->IntValue();
    }
    
    template <> u64 GetValue<u64>(const XMLAttribute * xmlValue)
    {
        return xmlValue->Unsigned64Value();
    }

    template <> i64 GetValue<i64>(const XMLAttribute * xmlValue)
    {
        return xmlValue->Int64Value();
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void Factory::serializeIntegerPropertyFromXML(IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem, core::uint _index) const
    {
        const XMLAttribute * xmlValue = _xmlElem->FindAttribute("value");
        if (nullptr != xmlValue)
        {
            T * p = (T *)(uint_ptr(_object) + _prop->GetOffset() + _index * _prop->GetSizeOf());
            *p = GetValue<T>(xmlValue);
        }
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void Factory::serializeIntegerPropertyToXML(const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem, core::uint _index) const
    {
        const T * p = (T *)(uint_ptr(_object) + _prop->GetOffset() + _index * _prop->GetSizeOf());
        _xmlElem->SetAttribute("value", *p);
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void Factory::serializeEnumPropertyFromXML(IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const
    {
        T * pEnum = (T *)(uint_ptr(_object) + _prop->GetOffset());
        const XMLAttribute * xmlValue = _xmlElem->FindAttribute("value");
        if (nullptr != xmlValue)
        {
            const auto val = xmlValue->Value();
            for (uint i = 0; i < _prop->GetEnumCount(); ++i)
            {
                if (!strcmp(val, _prop->GetEnumName(i).c_str()))
                    *pEnum = (T) (scalarTraits<T>::is_signed ? _prop->GetSignedEnumValue(i) : _prop->GetUnsignedEnumValue(i));
            }
        }
    }

    //--------------------------------------------------------------------------------------
    template<typename T> void Factory::serializeEnumPropertyToXML(const IObject * _object, const IProperty * _prop, XMLElem * _xmlElem) const
    {
        const T * pEnum = (const T *)(uint_ptr(_object) + _prop->GetOffset());
        for (uint i = 0; i < _prop->GetEnumCount(); ++i)
        {
            T temp = (T)(scalarTraits<T>::is_signed ? _prop->GetSignedEnumValue(i) : _prop->GetUnsignedEnumValue(i));
            if (*pEnum == temp)
            {
                _xmlElem->SetAttribute("value", _prop->GetEnumName(i).c_str());
                break;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    template <typename T> void Factory::serializeEnumFlagsPropertyFromXML(IObject * _object, const IProperty * _prop, const XMLElem * _xmlElem) const
    {
        T * pEnum = (T *)(uint_ptr(_object) + _prop->GetOffset());
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
                for (uint e = 0; e < _prop->GetEnumCount(); ++e)
                {
                    auto name = _prop->GetEnumName(e);
                    auto value = scalarTraits<T>::is_signed ? _prop->GetSignedEnumValue(e) : _prop->GetUnsignedEnumValue(e);

                    if (!strcmp(name.c_str(), temp))
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
        const T * pEnum = (const T *)(uint_ptr(_object) + _prop->GetOffset());
        string flags;
        bool first = true;
        for (uint i = 0; i < _prop->GetEnumCount(); ++i)
        {
            T temp = (T)(scalarTraits<T>::is_signed ? _prop->GetSignedEnumValue(i) : _prop->GetUnsignedEnumValue(i));
            if (0 != (*pEnum & temp))
            {
                if (!first)
                    flags += "|";
                else
                    first = false;
                flags += (string)_prop->GetEnumName(i);
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