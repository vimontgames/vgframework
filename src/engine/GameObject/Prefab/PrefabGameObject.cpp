#include "engine/Precomp.h"
#include "PrefabGameObject.h"
#include "engine/World/Prefab/PrefabScene.h"
#include "core/Object/DynamicProperties/DynamicProperties.h"
#include "engine/Component/Editor/Snap/SnapComponent.h"
#include "engine/Engine.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS_EX(PrefabGameObject, "PrefabGameObject", ClassDescFlags::GameObject | ClassDescFlags::UID);

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
    void PrefabGameObject::SetName(const string & _name)
    {
        super::SetName(_name);
        m_prefabResource.SetName(_name);
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
        const auto uid = _object->GetOriginalUID(false);
        if (uid)
        {
            for (uint i = 0; i < m_dynamicProperties.size(); ++i)
            {
                auto * propList = m_dynamicProperties[i];
                if (propList->GetUID() == uid)
                    return propList;
            }
        }
        else
        {
            VG_WARNING("[Prefab] Cannot get DynamicPropertyList for (%s)\"%s\" because it has no original UID", _object->GetClassName(), _object->GetName().c_str());
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    core::DynamicPropertyList * PrefabGameObject::createDynamicPropertyList(const core::IObject * _object)
    {
        VG_ASSERT(nullptr == GetDynamicPropertyList(_object));

        auto *& newPropList = m_dynamicProperties.emplace_back();

        // TODO : dedicated ctor?
        newPropList = new DynamicPropertyList(_object->GetName(), nullptr);
        newPropList->SetUID(_object->GetOriginalUID());
        newPropList->SetOriginalUID(0x0);
        newPropList->SetName(_object->GetName());
        
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
                if (prop->GetName() == _prop->GetName())
                    return prop;
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool PrefabGameObject::canOverrideProperty(const core::IObject * _object, const core::IProperty * _prop) const
    {
        const auto flags = _prop->GetFlags();

        if (asBool(PropertyFlags::Transient & flags))
            return false;

        if (!_object->GetUID(false))
        {
            VG_WARNING("[Prefab] Cannot override Property (%s)\"%s\" because Object \"%s\" has no UID", asString(_prop->GetType()).c_str(), _prop->GetName(), _object->GetName().c_str());
            return false;
        }

        const bool isFolder = asBool(PropertyFlags::IsFolder & flags);
        const bool isFile = asBool(PropertyFlags::IsFile & flags);

        const bool isEnumArray = asBool(PropertyFlags::EnumArray & flags);
        const bool isColor = asBool(PropertyFlags::Color & flags);

        const bool optional = asBool(PropertyFlags::Optional & flags); // TODO: temp

        switch (_prop->GetType())
        {
            default:
                return false;

            case PropertyType::Bool:
            {
                return true;
            }
            break;

            case PropertyType::String:
            {
                if (isFile || isFolder)
                    return false;

                return true;
            }
            break;

            case PropertyType::Uint8:
            case PropertyType::EnumU8:
            case PropertyType::EnumFlagsU8:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Uint16:
            case PropertyType::EnumU16:
            case PropertyType::EnumFlagsU16:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Uint32:
            case PropertyType::EnumU32:
            case PropertyType::EnumFlagsU32:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Uint2:
            case PropertyType::Uint3:
            case PropertyType::Uint4:
            case PropertyType::Int2:
            case PropertyType::Int3:
            case PropertyType::Int4:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Uint64:
            case PropertyType::EnumU64:
            case PropertyType::EnumFlagsU64:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Int8:
            case PropertyType::EnumI8:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Int16:
            case PropertyType::EnumI16:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Int32:
            case PropertyType::EnumI32:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Int64:
            case PropertyType::EnumI64:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Float:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Float2:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Float3:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Float4:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::Float4x4:
            {
                if (isEnumArray)
                    return false;

                return true;
            }
            break;

            case PropertyType::ObjectHandle:
            {
                return true;
            }
            break;       

            case PropertyType::Resource:
            {
                return true;
            }
            break;
        }
    }

    //--------------------------------------------------------------------------------------
    core::DynamicProperty * PrefabGameObject::createDynamicProperty(const core::IObject * _object, const core::IProperty * _prop)
    {
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
           
            auto & newDynProp = propList->m_properties.emplace_back();

            switch (_prop->GetType())
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_prop->GetType());
                    return nullptr;

                case PropertyType::ObjectHandle:
                    newDynProp = new DynamicPropertyObjectHandle(_prop->GetName());
                    break;

                case PropertyType::Resource:
                    newDynProp = new DynamicPropertyResource(_prop->GetName());
                    break;

                case PropertyType::Bool:
                    newDynProp = new DynamicPropertyBool(_prop->GetName());
                    break;

                case PropertyType::String:
                    newDynProp = new DynamicPropertyString(_prop->GetName());
                    break;

                case PropertyType::Float:
                    newDynProp = new DynamicPropertyFloat(_prop->GetName());
                    break;

                case PropertyType::Float2:
                    newDynProp = new DynamicPropertyFloat2(_prop->GetName());
                    break;

                case PropertyType::Float3:
                    newDynProp = new DynamicPropertyFloat3(_prop->GetName());
                    break;

                case PropertyType::Float4:
                    newDynProp = new DynamicPropertyFloat4(_prop->GetName());
                    break;  

                case PropertyType::Float4x4:
                    newDynProp = new DynamicPropertyFloat4x4(_prop->GetName());
                    break;

                case PropertyType::Uint8:
                case PropertyType::EnumU8:
                case PropertyType::EnumFlagsU8:
                    newDynProp = new DynamicPropertyU8(_prop->GetName());
                    break;

                case PropertyType::Uint16:
                case PropertyType::EnumU16:
                case PropertyType::EnumFlagsU16:
                    newDynProp = new DynamicPropertyU16(_prop->GetName());
                    break;

                case PropertyType::Uint64:
                case PropertyType::EnumU64:
                case PropertyType::EnumFlagsU64:
                    newDynProp = new DynamicPropertyU64(_prop->GetName());
                    break;

                case PropertyType::Uint32:
                case PropertyType::EnumU32:
                case PropertyType::EnumFlagsU32:
                    newDynProp = new DynamicPropertyU32(_prop->GetName());
                    break;

                case PropertyType::Uint2:
                    newDynProp = new DynamicPropertyUInt2(_prop->GetName());
                    break;

                case PropertyType::Uint3:
                    newDynProp = new DynamicPropertyUInt3(_prop->GetName());
                    break;

                case PropertyType::Uint4:
                    newDynProp = new DynamicPropertyUInt4(_prop->GetName());
                    break;

                case PropertyType::Int2:
                    newDynProp = new DynamicPropertyInt2(_prop->GetName());
                    break;

                case PropertyType::Int3:
                    newDynProp = new DynamicPropertyInt3(_prop->GetName());
                    break;

                case PropertyType::Int4:
                    newDynProp = new DynamicPropertyInt4(_prop->GetName());
                    break;

                case PropertyType::Int8:
                case PropertyType::EnumI8:
                    newDynProp = new DynamicPropertyI8(_prop->GetName());
                    break;

                case PropertyType::Int16:
                case PropertyType::EnumI16:
                    newDynProp = new DynamicPropertyI16(_prop->GetName());
                    break;

                case PropertyType::Int32:
                case PropertyType::EnumI32:
                    newDynProp = new DynamicPropertyI32(_prop->GetName());
                    break;

                case PropertyType::Int64:
                case PropertyType::EnumI64:
                    newDynProp = new DynamicPropertyI64(_prop->GetName());
                    break;
            }

            // update (TODO: make proper method!)
            VG_VERIFY(propList->GetProperty(_prop));

            newDynProp->setParent(this);
            newDynProp->BackupOriginalValue(_object, _prop);
            newDynProp->SetOverrideInitValue(_object, _prop);

            return newDynProp;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    // In case of Prefab during editition, we copy we simply create a new PrefabGameObject and copy Prefab path.
    // At runtime, a full copy is made so that the object can be used immediately
    //--------------------------------------------------------------------------------------
    IObject * PrefabGameObject::Instanciate() const
    {
        if (Engine::get()->IsPlaying())
        {
            return super::Instanciate();
        }
        else
        {
            auto * factory = Kernel::getFactory();
            return factory->Instanciate(this, nullptr, CopyPropertyFlags::NoChildren);
        }
    }

    //--------------------------------------------------------------------------------------
    void PrefabGameObject::OnResourceLoaded(IResource * _resource)
    {
        if (_resource == &m_prefabResource)
        {
            VG_PROFILE_CPU("PrefabGameObject");

            // Instanciate the Prefab
            if (PrefabScene * prefabScene = VG_SAFE_STATIC_CAST(PrefabScene, _resource->GetObject()))
            {
                IGameObject * prefabRoot = prefabScene->GetRoot();

                // Load Prefab as "not opened" in editor
                SetObjectFlags(ObjectFlags::Opened, false);

                auto & prefabChildren = GetChildren();
                if (prefabChildren.size() == 0)
                {
                    IGameObject * instance = VG_SAFE_STATIC_CAST(IGameObject, prefabRoot->Instanciate());
                    instance->SetObjectFlags(ObjectFlags::NotSerialized, true);
                    instance->SetParent(this);

                    // If an object in a Prefab references another object from the same Prefab using UID, 
                    // then it must be "patched" so that it becomes an UID in the instanced Prefab too.
                    patchPrefabGameObjectUIDs(instance, instance);

                    // Unless it's explicitly overridden in the Prefab instance of course.
                    OverrideGameObjectProperties((GameObject*)instance, nullptr);

                    AddChild(instance);

                    // Force Prefab snap
                    if (SnapComponent * snapComp = instance->GetComponentInChildrenT<SnapComponent>())
                        snapComp->snap();

                    VG_SAFE_RELEASE(instance);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void PrefabGameObject::patchPrefabGameObjectUIDs(const IObject * _root, IObject * _object)
    {
        if (nullptr == _object)
            return;

        if (const auto * classDesc = _object->GetClassDesc())
        {
            const auto propCount = classDesc->GetPropertyCount();
            for (uint i = 0; i < propCount; ++i)
            {
                const auto * prop = classDesc->GetPropertyByIndex(i);
                switch (prop->GetType())
                {
                    default:
                        break;

                    case PropertyType::ObjectHandle:
                    {
                        if (ObjectHandle * handle = prop->GetPropertyObjectHandle(_object))
                        {
                            if (0x0 != handle->getUID())
                            {
                                const IObject * newTarget = _root->FindByOriginalUID(handle->getUID());
                                if (newTarget)
                                {
                                    VG_INFO("[Prefab] Patched ObjectHandle UID 0x%08X from Property \"%s\" in Object \"%s\" of class %s to UID 0x%08X", handle->getUID(), prop->GetName(), _object->GetName().c_str(), classDesc->GetClassName(), newTarget->GetUID());
                                    handle->setUID(newTarget->GetUID());
                                }
                                else
                                {
                                    VG_WARNING("[Prefab] Could not patch ObjectHandle UID 0x%08X from Property \"%s\" in Object \"%s\" of class %s", handle->getUID(), prop->GetName(), _object->GetName().c_str(), classDesc->GetClassName());
                                }
                            }
                        }
                    }
                    break;

                    case PropertyType::ObjectPtrVector:
                    {
                        if (auto * objVec = prop->GetPropertyObjectPtrVector(_object))
                        {
                            for (auto * obj : *objVec)
                                patchPrefabGameObjectUIDs(_root, obj);
                        }
                    }
                    break;
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

                            case PropertyType::ResourcePtr:
                            case PropertyType::ResourcePtrVector:
                                VG_ASSERT_ENUM_NOT_IMPLEMENTED(propType);
                            break;

                            case PropertyType::Object:
                            {
                                Object * obj = (Object *)prop->GetPropertyObject(_object);
                                if (nullptr != obj)
                                {
                                    if (auto * found = find(obj, _uid))
                                        return found;
                                }
                            }
                            break;

                            case PropertyType::Resource:
                            {
                                Object * res = (Object *)prop->GetPropertyResource(_object);
                                if (nullptr != res)
                                {
                                    if (auto * found = find(res, _uid))
                                        return found;
                                }
                            }
                            break;

                            case PropertyType::ResourceVector:
                            {
                                const auto count = prop->GetPropertyResourceVectorCount(_object);
                                for (uint j = 0; j < count; ++j)
                                {
                                    Resource * res = (Resource *)prop->GetPropertyResourceVectorElement(_object, j);

                                    if (auto * found = find(res, _uid))
                                        return found;
                                }
                            }
                            break;

                            case PropertyType::ObjectPtr:
                            {
                                Object ** obj = (Object **)prop->GetPropertyObjectPtr(_object);
                                if (nullptr != *obj)
                                {
                                    if (auto * found = find(*obj, _uid))
                                        return found;
                                }
                            }
                            break;

                            case PropertyType::ObjectPtrVector:
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
        bool found = false;

        VG_PROFILE_CPU("OverrideProperties");

        for (uint j = 0; j < m_dynamicProperties.size(); ++j)
        {
            auto & propList = m_dynamicProperties[j];

            //VG_INFO("[Prefab] Override %u properties for GameObject \"%s\" in Prefab \"%s\"", propList->m_properties.size(), _gameObject->getName().c_str(), getName().c_str());
            
            if (auto * obj = find(_gameObject, propList->GetUID()))
            {
                auto * classDesc = obj->GetClassDesc();

                //VG_INFO("[Prefab] Could find GUID 0x%08X in GameObject \"%s\"", propList->GetUID(), _gameObject->getName().c_str());

                for (uint i = 0; i < propList->m_properties.size(); ++i)
                {
                    auto & overrideProp = propList->m_properties[i];

                    if (nullptr == _dynProp || _dynProp->GetProperty() == overrideProp->GetProperty())
                    {
                        if (IProperty * origProp = classDesc->GetPropertyByName(overrideProp->GetName().c_str()))
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
                }
            }
            else
            {
                VG_WARNING("[Prefab] Could not find GUID %u (0x%08X) in Prefab \"%s\"", propList->GetUID(), propList->GetUID(), m_prefabResource.GetResourcePath().c_str());
            }
        }

        if (nullptr != _dynProp && !found)
            VG_WARNING("[Prefab] Could not find property \"%s\" in Prefab \"%s\"", _dynProp->GetName().c_str(), m_prefabResource.GetResourcePath().c_str());
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
    void PrefabGameObject::OnResourceUnloaded(IResource * _resource)
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