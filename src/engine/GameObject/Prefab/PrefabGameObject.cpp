#include "engine/Precomp.h"
#include "PrefabGameObject.h"
#include "engine/World/Prefab/PrefabScene.h"

#include "core/Object/DynamicProperties/DynamicPropertyList.h"
#include "core/Object/DynamicProperties/DynamicPropertyType/DynamicPropertyString.h"
#include "core/Object/DynamicProperties/DynamicPropertyType/DynamicPropertyFloat4.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS_EX(PrefabGameObject, "PrefabGameObject", IClassDesc::Flags::GameObject);

    //--------------------------------------------------------------------------------------
    bool PrefabGameObject::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyResource(PrefabGameObject, m_prefabResource, "File");
        registerPropertyObjectPtrVector(PrefabGameObject, m_gameObjects, "GameObjects");
        
        return true;
    }

    //--------------------------------------------------------------------------------------
    PrefabGameObject::PrefabGameObject(const core::string & _name, IObject * _parent) :
        GameObject(_name, _parent)
    {
        m_prefabResource.setParent(this);
        setObjectFlags(ObjectFlags::Prefab, true);
    }

    //--------------------------------------------------------------------------------------
    PrefabGameObject::~PrefabGameObject()
    {
        for (uint i = 0; i < m_gameObjects.size(); ++i)
            VG_SAFE_RELEASE(m_gameObjects[i]);
        m_gameObjects.clear();
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
        const auto uid = _object->GetUID();
        for (uint i = 0; i < m_gameObjects.size(); ++i)
        {
            auto * propList = m_gameObjects[i];
            if (propList->GetUID() == uid)
                return propList;
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    core::DynamicPropertyList * PrefabGameObject::createDynamicPropertyList(const core::IObject * _object)
    {
        VG_ASSERT(nullptr == GetDynamicPropertyList(_object));

        auto *& newPropList = m_gameObjects.push_empty();

        // TODO : dedicated ctor?
        newPropList = new DynamicPropertyList(_object->getName(), nullptr);
        newPropList->SetUID(_object->GetUID());
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
                if (prop->getName() == _prop->getName())
                    return prop;
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool PrefabGameObject::canOverrideProperty(const core::IObject * _object, const core::IProperty * _prop) const
    {
        const auto flags = _prop->getFlags();

        const bool isFolder = asBool(IProperty::Flags::IsFolder & flags);
        const bool isFile = asBool(IProperty::Flags::IsFile & flags);

        const bool isEnumArray = asBool(IProperty::Flags::EnumArray & flags);
        const bool isColor = asBool(IProperty::Flags::Color & flags);

        switch (_prop->getType())
        {
            default:
                return false;

            case IProperty::Type::String:
            {
                if (isFile || isFolder)
                    return false;

                return true;
            }
            break;

            case IProperty::Type::Float4:
            {
                if (isEnumArray || !isColor)
                    return false;

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
           
            auto & newDynProp = propList->m_properties.push_empty();

            switch (_prop->getType())
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_prop->getType());
                    return nullptr;

                case IProperty::Type::String:
                    newDynProp = new DynamicPropertyString(_object, _prop);
                    break;

                case IProperty::Type::Float4:
                    newDynProp = new DynamicPropertyFloat4(_object, _prop);
                    break;
            }

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
            if (PrefabScene * prefabScene = VG_SAFE_STATIC_CAST(PrefabScene, _resource->getObject()))
            {
                IGameObject * prefabRoot = prefabScene->GetRoot();

                auto & prefabChildren = GetChildren();
                if (prefabChildren.size() == 0)
                {
                    IGameObject * instance = VG_SAFE_STATIC_CAST(IGameObject, prefabRoot->Instanciate());
                    instance->SetObjectFlags(ObjectFlags::NotSerialized, true);
                    instance->setParent(this);

                    OverrideGameObjectProperties((GameObject*)instance);

                    AddChild(instance);
                    VG_SAFE_RELEASE(instance);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void PrefabGameObject::OverrideGameObjectProperties(IGameObject * _gameObject, const IProperty * _prop)
    {
        DynamicPropertyList * propList = nullptr;
        const auto uid = _gameObject->GetUID();
        for (uint i = 0; i < m_gameObjects.size(); ++i)
        {
            auto & props = m_gameObjects[i];
            if (props->GetUID() == uid)
            {
                propList = props;
                break;
            }
        }

        if (propList)
        {
            VG_INFO("[Prefab] Override %u properties for GameObject \"%s\" in Prefab \"%s\"", propList->m_properties.size(), _gameObject->getName().c_str(), getName().c_str());
            auto * classDesc = _gameObject->GetClassDesc();

            for (uint i = 0; i < propList->m_properties.size(); ++i)
            {
                auto & overrideProp = propList->m_properties[i];

                if (nullptr == _prop || _prop == overrideProp->GetProperty())
                {
                    if (IProperty * origProp = classDesc->GetPropertyByName(overrideProp->getName().c_str()))
                        overrideProp->Set(_gameObject, origProp);
                }
            }
        }

        auto & children = _gameObject->GetChildren();
        for (uint i = 0; i < children.size(); ++i)
            OverrideGameObjectProperties(children[i], _prop);
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
                prefabChild->setParent(nullptr);
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

    ////--------------------------------------------------------------------------------------
    //void PrefabGameObject::OnPlay()
    //{
    //    
    //}
    //
    ////--------------------------------------------------------------------------------------
    //void PrefabGameObject::OnStop()
    //{
    //   
    //}
}