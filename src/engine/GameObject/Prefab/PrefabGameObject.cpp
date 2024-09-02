#include "engine/Precomp.h"
#include "PrefabGameObject.h"
#include "engine/World/Prefab/PrefabScene.h"
#include "core/Object/DynamicProperties/DynamicProperties.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS_EX(PrefabGameObject, "PrefabGameObject", IClassDesc::Flags::GameObject | IClassDesc::Flags::UID);

    //--------------------------------------------------------------------------------------
    bool PrefabGameObject::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyResource(PrefabGameObject, m_prefabResource, "File");
        registerPropertyObjectPtrVector(PrefabGameObject, m_dynamicProperties, "Property Overrides");
        
        return true;
    }

    //--------------------------------------------------------------------------------------
    PrefabGameObject::PrefabGameObject(const core::string & _name, IObject * _parent) :
        GameObject(_name, _parent)
    {
        m_prefabResource.SetParent(this);
        setObjectFlags(ObjectFlags::Prefab, true);
    }

    //--------------------------------------------------------------------------------------
    PrefabGameObject::~PrefabGameObject()
    {
        for (uint i = 0; i < m_dynamicProperties.size(); ++i)
            VG_SAFE_RELEASE(m_dynamicProperties[i]);
        m_dynamicProperties.clear();
    }

    //--------------------------------------------------------------------------------------
    bool PrefabGameObject::IsPrefab() const
    {
        return true;
    }

    //--------------------------------------------------------------------------------------
    core::IResource * PrefabGameObject::GetPrefabResource() const
    {
        VG_ASSERT(IsPrefab());
        return (core::IResource *) &m_prefabResource;
    }

    //--------------------------------------------------------------------------------------
    core::IDynamicPropertyList * PrefabGameObject::GetDynamicPropertyList(const core::IObject * _object) const
    {
        return getDynamicPropertyList(_object);
    }

    //--------------------------------------------------------------------------------------
    core::IDynamicPropertyList * PrefabGameObject::CreateDynamicPropertyList(const core::IObject * _object)
    {
        return createDynamicPropertyList(_object);
    }

    //--------------------------------------------------------------------------------------
    core::IDynamicProperty * PrefabGameObject::GetDynamicProperty(const core::IObject * _object, const core::IProperty * _prop) const
    {
        return getDynamicProperty(_object, _prop);
    }

    //--------------------------------------------------------------------------------------
    bool PrefabGameObject::CanOverrideProperty(const core::IObject * _object, const core::IProperty * _prop) const
    {
        return canOverrideProperty(_object, _prop);
    }

    //--------------------------------------------------------------------------------------
    core::IDynamicProperty * PrefabGameObject::CreateDynamicProperty(const IObject * _object, const IProperty * _prop)
    {
        return createDynamicProperty(_object, _prop);
    }

    //--------------------------------------------------------------------------------------
    core::DynamicPropertyList * PrefabGameObject::getDynamicPropertyList(const core::IObject * _object) const
    {
        const auto uid = _object->GetOriginalUID();
        if (uid)
        {
            for (uint i = 0; i < m_dynamicProperties.size(); ++i)
            {
                auto * propList = m_dynamicProperties[i];
                if (propList->GetUID() == uid)
                    return propList;
            }
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    core::DynamicPropertyList * PrefabGameObject::createDynamicPropertyList(const core::IObject * _object)
    {
        VG_ASSERT(nullptr == GetDynamicPropertyList(_object));

        auto *& newPropList = m_dynamicProperties.push_empty();

        // TODO : dedicated ctor?
        newPropList = new DynamicPropertyList(_object->getName(), nullptr);
        newPropList->SetUID(_object->GetOriginalUID());
        newPropList->SetOriginalUID(0x0);
        newPropList->setName(_object->getName());
        
        return newPropList;
    }

    //--------------------------------------------------------------------------------------
    core::DynamicProperty * PrefabGameObject::getDynamicProperty(const core::IObject * _object, const core::IProperty * _prop) const
    {
        if (auto * propList = getDynamicPropertyList(_object))
        {
            for (uint i = 0; i < propList->m_properties.size(); ++i)
            {
                auto & prop = propList->m_properties[i];
                if (prop->getName() == _prop->GetName())
                    return prop;
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool PrefabGameObject::canOverrideProperty(const core::IObject * _object, const core::IProperty * _prop) const
    {
        const auto flags = _prop->GetFlags();

        const bool isFolder = asBool(IProperty::Flags::IsFolder & flags);
        const bool isFile = asBool(IProperty::Flags::IsFile & flags);

        const bool isEnumArray = asBool(IProperty::Flags::EnumArray & flags);
        const bool isColor = asBool(IProperty::Flags::Color & flags);

        const bool optional = asBool(IProperty::Flags::Optional & flags); // TODO: temp

        switch (_prop->GetType())
        {
            default:
                return false;

            case IProperty::Type::Bool:
            {
                return true;
            }
            break;

            case IProperty::Type::String:
            {
                if (isFile || isFolder)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Uint8:
            case IProperty::Type::EnumU8:
            case IProperty::Type::EnumFlagsU8:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Uint16:
            case IProperty::Type::EnumU16:
            case IProperty::Type::EnumFlagsU16:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Uint32:
            case IProperty::Type::EnumU32:
            case IProperty::Type::EnumFlagsU32:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Uint2:
            case IProperty::Type::Uint3:
            case IProperty::Type::Uint4:
            case IProperty::Type::Int2:
            case IProperty::Type::Int3:
            case IProperty::Type::Int4:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Uint64:
            case IProperty::Type::EnumU64:
            case IProperty::Type::EnumFlagsU64:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Int8:
            case IProperty::Type::EnumI8:
            case IProperty::Type::EnumFlagsI8:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Int16:
            case IProperty::Type::EnumI16:
            case IProperty::Type::EnumFlagsI16:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Int32:
            case IProperty::Type::EnumI32:
            case IProperty::Type::EnumFlagsI32:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Int64:
            case IProperty::Type::EnumI64:
            case IProperty::Type::EnumFlagsI64:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Float:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Float2:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Float3:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Float4:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Float4x4:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::ObjectHandle:
            {
                return true;
            }
            break;       

            case IProperty::Type::Resource:
            {
                return true;
            }
            break;
        }
    }

    //--------------------------------------------------------------------------------------
    core::DynamicProperty * PrefabGameObject::createDynamicProperty(const core::IObject * _object, const core::IProperty * _prop)
    {
        //if (_prop->getType() == IProperty::Type::Resource)
        //{
        //    _object = _prop->GetPropertyResource(_object);
        //    auto classDesc = _object->GetClassDesc();
        //    _prop = classDesc->GetPropertyByName("m_resourcePath");
        //}

        if (auto * dynProp = getDynamicProperty(_object, _prop))
            return dynProp;

        VG_ASSERT(canOverrideProperty(_object, _prop));

        DynamicPropertyList * propList = getDynamicPropertyList(_object);
        if (nullptr == propList)
            propList = createDynamicPropertyList(_object);

        if (nullptr != propList)
        {
            if (auto * dynProp = getDynamicProperty(_object, _prop))
                return dynProp;
           
            auto & newDynProp = propList->m_properties.push_empty();

            switch (_prop->GetType())
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_prop->GetType());
                    return nullptr;

                case IProperty::Type::ObjectHandle:
                    newDynProp = new DynamicPropertyObjectHandle(_prop->GetName());
                    break;

                case IProperty::Type::Resource:
                    newDynProp = new DynamicPropertyResource(_prop->GetName());
                    break;

                case IProperty::Type::Bool:
                    newDynProp = new DynamicPropertyBool(_prop->GetName());
                    break;

                case IProperty::Type::String:
                    newDynProp = new DynamicPropertyString(_prop->GetName());
                    break;

                case IProperty::Type::Float:
                    newDynProp = new DynamicPropertyFloat(_prop->GetName());
                    break;

                case IProperty::Type::Float2:
                    newDynProp = new DynamicPropertyFloat2(_prop->GetName());
                    break;

                case IProperty::Type::Float3:
                    newDynProp = new DynamicPropertyFloat3(_prop->GetName());
                    break;

                case IProperty::Type::Float4:
                    newDynProp = new DynamicPropertyFloat4(_prop->GetName());
                    break;  

                case IProperty::Type::Float4x4:
                    newDynProp = new DynamicPropertyFloat4x4(_prop->GetName());
                    break;

                case IProperty::Type::Uint8:
                case IProperty::Type::EnumU8:
                case IProperty::Type::EnumFlagsU8:
                    newDynProp = new DynamicPropertyU8(_prop->GetName());
                    break;

                case IProperty::Type::Uint16:
                case IProperty::Type::EnumU16:
                case IProperty::Type::EnumFlagsU16:
                    newDynProp = new DynamicPropertyU16(_prop->GetName());
                    break;

                case IProperty::Type::Uint64:
                case IProperty::Type::EnumU64:
                case IProperty::Type::EnumFlagsU64:
                    newDynProp = new DynamicPropertyU64(_prop->GetName());
                    break;

                case IProperty::Type::Uint32:
                case IProperty::Type::EnumU32:
                case IProperty::Type::EnumFlagsU32:
                    newDynProp = new DynamicPropertyU32(_prop->GetName());
                    break;

                case IProperty::Type::Uint2:
                    newDynProp = new DynamicPropertyUInt2(_prop->GetName());
                    break;

                case IProperty::Type::Uint3:
                    newDynProp = new DynamicPropertyUInt3(_prop->GetName());
                    break;

                case IProperty::Type::Uint4:
                    newDynProp = new DynamicPropertyUInt4(_prop->GetName());
                    break;

                case IProperty::Type::Int2:
                    newDynProp = new DynamicPropertyInt2(_prop->GetName());
                    break;

                case IProperty::Type::Int3:
                    newDynProp = new DynamicPropertyInt3(_prop->GetName());
                    break;

                case IProperty::Type::Int4:
                    newDynProp = new DynamicPropertyInt4(_prop->GetName());
                    break;

                case IProperty::Type::Int8:
                case IProperty::Type::EnumI8:
                case IProperty::Type::EnumFlagsI8:
                    newDynProp = new DynamicPropertyI8(_prop->GetName());
                    break;

                case IProperty::Type::Int16:
                case IProperty::Type::EnumI16:
                case IProperty::Type::EnumFlagsI16:
                    newDynProp = new DynamicPropertyI16(_prop->GetName());
                    break;

                case IProperty::Type::Int32:
                case IProperty::Type::EnumI32:
                case IProperty::Type::EnumFlagsI32:
                    newDynProp = new DynamicPropertyI32(_prop->GetName());
                    break;

                case IProperty::Type::Int64:
                case IProperty::Type::EnumI64:
                case IProperty::Type::EnumFlagsI64:
                    newDynProp = new DynamicPropertyI64(_prop->GetName());
                    break;
            }

            // update (TODO: make proper method!)
            VG_VERIFY(propList->GetProperty(_prop));

            newDynProp->BackupOriginalValue(_object, _prop);
            newDynProp->SetOverrideInitValue(_object, _prop);

            return newDynProp;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void PrefabGameObject::onResourceLoaded(IResource * _resource)
    {
        if (_resource == &m_prefabResource)
        {
            // Instanciate the Prefab
            if (PrefabScene * prefabScene = VG_SAFE_STATIC_CAST(PrefabScene, _resource->GetObject()))
            {
                IGameObject * prefabRoot = prefabScene->GetRoot();

                auto & prefabChildren = GetChildren();
                if (prefabChildren.size() == 0)
                {
                    IGameObject * instance = VG_SAFE_STATIC_CAST(IGameObject, prefabRoot->Instanciate());
                    instance->SetObjectFlags(ObjectFlags::NotSerialized, true);
                    instance->SetParent(this);

                    OverrideGameObjectProperties((GameObject*)instance, nullptr);

                    AddChild(instance);
                    VG_SAFE_RELEASE(instance);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    Object * find(Object * _object, UID _uid)
    {
        VG_ASSERT(_uid);
        if (_uid)
        {
            if (!_object->HasValidUID())
                return nullptr;

            auto objUID = _object->GetOriginalUID(false);
            if (objUID == _uid)
            {
                return _object;
            }
            else
            {
                const auto * classDesc = _object->GetClassDesc();
                if (classDesc)
                {
                    const auto propCount = classDesc->GetPropertyCount();
                    for (uint i = 0; i < propCount; ++i)
                    {
                        const auto * prop = classDesc->GetPropertyByIndex(i);
                        const auto propType = prop->GetType();

                        switch (propType)
                        {
                            default:
                                break;

                            case IProperty::Type::ObjectPtrVector:
                            {
                                const auto vec = prop->GetPropertyObjectPtrVector(_object);
                                for (uint j = 0; j < vec->size(); ++j)
                                {
                                    Object * obj = (Object*)(*vec)[j];

                                    if (auto * found = find(obj, _uid))
                                        return found;
                                }
                            }
                            break;

                            case IProperty::Type::ObjectPtr:
                            {
                                Object ** obj = (Object **)prop->GetPropertyObjectPtr(_object);
                                if (nullptr != *obj)
                                {
                                    if (auto * found = find(*obj, _uid))
                                        return found;
                                }
                            }
                            break;
                        }
                    }                    
                }
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void PrefabGameObject::OverrideGameObjectProperties(IGameObject * _gameObject, const IDynamicProperty * _dynProp)
    {        
        for (uint i = 0; i < m_dynamicProperties.size(); ++i)
        {
            auto & propList = m_dynamicProperties[i];

            //VG_INFO("[Prefab] Override %u properties for GameObject \"%s\" in Prefab \"%s\"", propList->m_properties.size(), _gameObject->getName().c_str(), getName().c_str());
            
            if (auto * obj = find(_gameObject, propList->GetUID()))
            {
                auto * classDesc = obj->GetClassDesc();

                //VG_INFO("[Prefab] Could find GUID 0x%08X in GameObject \"%s\"", propList->GetUID(), _gameObject->getName().c_str());

                for (uint i = 0; i < propList->m_properties.size(); ++i)
                {
                    auto & overrideProp = propList->m_properties[i];

                    bool found = true;

                    if (nullptr == _dynProp || _dynProp->GetProperty() == overrideProp->GetProperty())
                    {
                        if (IProperty * origProp = classDesc->GetPropertyByName(overrideProp->getName().c_str()))
                        {
                            if (_dynProp == nullptr)
                                overrideProp->BackupOriginalValue(obj, origProp);

                            if (overrideProp->IsEnable())
                                overrideProp->ApplyOverride(obj, origProp);
                            else
                                overrideProp->RestoreOriginalValue(obj, origProp);  

                            obj->OnPropertyChanged(obj, *origProp, true);
                            found = true;
                        }
                    }

                    if (!found)
                        VG_WARNING("[Prefab] Could not find property \"%s\" in Prefab \"%s\"", overrideProp->getName().c_str(), m_prefabResource.GetResourcePath().c_str());
                }
            }
            else
            {
                VG_WARNING("[Prefab] Could not find GUID %u (0x%08X) in Prefab \"%s\"", propList->GetUID(), propList->GetUID(), m_prefabResource.GetResourcePath().c_str());
            }
        }
    }

    //--------------------------------------------------------------------------------------
    // _override = true => Enable property override, Set to Override value
    // _override = false => Disable property override, Reset to original value
    //--------------------------------------------------------------------------------------
    bool PrefabGameObject::ToggleOverride(const IObject * _object, const IProperty * _prop, bool _override)
    {
        if (_override)
        {
            if (auto * propOverride = CreateDynamicProperty(_object, _prop))
            {
                propOverride->Enable(true);

                auto & children = GetChildren();
                for (uint i = 0; i < children.size(); ++i)
                    OverrideGameObjectProperties(children[i], propOverride);
                return true;
            }

            return false;
        }
        else
        {
            if (auto * propOverride = GetDynamicProperty(_object, _prop))
            {
                propOverride->Enable(false);

                auto & children = GetChildren();
                for (uint i = 0; i < children.size(); ++i)
                    OverrideGameObjectProperties(children[i], propOverride);
                return true;
            }

            return false;
        }
    }

    //--------------------------------------------------------------------------------------
    void PrefabGameObject::onResourceUnloaded(IResource * _resource)
    {
        if (_resource == &m_prefabResource)
        {
            // Destroy the Prefab instances
            auto & prefabChildren = GetChildren();

            if (prefabChildren.size() > 0)
            {
                auto * prefabChild = prefabChildren[0];
                prefabChild->SetParent(nullptr);
                RemoveChild(prefabChild, false);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void PrefabGameObject::OnLoad()
    {
        super::OnLoad();
        setObjectFlags(ObjectFlags::Prefab, true);
    }

    //--------------------------------------------------------------------------------------
    void PrefabGameObject::OnPlay()
    {
        super::OnPlay();
    }
    
    //--------------------------------------------------------------------------------------
    void PrefabGameObject::OnStop()
    {
        super::OnStop();
    }
}