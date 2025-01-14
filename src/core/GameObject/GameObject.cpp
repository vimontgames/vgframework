#include "core/Precomp.h"
#include "GameObject.h"
#include "core/IBehaviour.h"
#include "core/ISelection.h"
#include "core/IProfiler.h"
#include "core/IBaseScene.h"
#include "core/Kernel.h"
#include "core/Component/Component.h"
#include "core/Misc/AABB/AABB.h"
#include "core/Object/AutoRegisterClass.h"  

#include "renderer/IGraphicInstance.h"

#if !VG_ENABLE_INLINE
#include "GameObject.inl"
#endif

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS_EX(GameObject, "GameObject", ClassDescFlags::GameObject | ClassDescFlags::UID);

    //--------------------------------------------------------------------------------------
    bool GameObject::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        // Hide GameObject name
        setPropertyFlag(GameObject, m_name, PropertyFlags::Hidden, false);

        registerPropertyEnumBitfield(GameObject, Tag, m_tags, "Tags");
        setPropertyDescription(GameObject, m_tags, "GameObjects can use Tags to categorize object (e.g., Player, Ennemy ...)")

        registerPropertyObjectPtrVector(GameObject, m_components, "Components");
        registerPropertyObjectPtrVectorEx(GameObject, m_children, "Children", PropertyFlags::Hidden);

        return true;
    }

    //--------------------------------------------------------------------------------------
    GameObject::GameObject(const core::string & _name, IObject * _parent) :
        IGameObject(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    GameObject::~GameObject()
    {
        // Remove GameObject from selection when deleted
        ISelection * selection = Kernel::getSelection(false);
        if (nullptr != selection && selection->IsSelectedObject(this))
            selection->Remove(this);

        for (uint i = 0; i < m_children.size(); ++i)
            VG_SAFE_RELEASE(m_children[i]);
        m_children.clear();

        for (uint i = 0; i < m_graphicInstances.size(); ++i)
            VG_SAFE_RELEASE(m_graphicInstances[i]);
        m_graphicInstances.clear();

        for (uint i = 0; i < m_components.size(); ++i)
            VG_SAFE_RELEASE(m_components[i]);
        m_components.clear();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnLoad()
    {
        super::OnLoad();
        RegisterUID();
        recomputeUpdateFlags();
        sortComponents();
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::RegisterUID()
    {
        bool anyUIDCreated = super::RegisterUID();

        for (uint i = 0; i < m_components.size(); ++i)
        {
            auto * component = m_components[i];
            if (nullptr != component)
            {
                if (component->RegisterUID())
                    anyUIDCreated = true;
            }
        }

        return anyUIDCreated;
    }

    //--------------------------------------------------------------------------------------
    // Sort component by priority defined in their ClassDesc
    //--------------------------------------------------------------------------------------
    void GameObject::sortComponents()
    {
        sort(m_components.begin(), m_components.end(), [](Component * a, Component * b)
        {
            return a->GetClassDesc()->GetPriority() < b->GetClassDesc()->GetPriority();
        }
        );
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnPlay()
    {
        if (isEnabled())
        {
            super::OnPlay();

            for (uint i = 0; i < m_components.size(); ++i)
            {
                auto * component = m_components[i];
                if (component->isEnabled())
                    component->OnPlay();
            }

            for (uint i = 0; i < m_children.size(); ++i)
                m_children[i]->OnPlay();
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnStop()
    {
        if (isEnabled())
        {
            for (uint i = 0; i < m_components.size(); ++i)
            {
                auto * component = m_components[i];
                if (component->isEnabled())
                    component->OnStop();
            }

            for (uint i = 0; i < m_children.size(); ++i)
                m_children[i]->OnStop();

            super::OnStop();
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnEnable()
    {
        if (isEnabled())
        {
            super::OnEnable();

            for (uint i = 0; i < m_components.size(); ++i)
            {
                auto * component = m_components[i];
                if (component->isEnabled())
                    component->OnEnable();
            }

            for (uint i = 0; i < m_children.size(); ++i)
                m_children[i]->OnEnable();
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnDisable()
    {
        for (uint i = 0; i < m_children.size(); ++i)
            m_children[i]->OnDisable();

        for (uint i = 0; i < m_components.size(); ++i)
            m_components[i]->OnDisable();

        super::OnDisable();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::SetParent(IObject * _parent)
    {
        super::SetParent(_parent);
        recomputeUpdateFlags();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::SetInstanceFlags(InstanceFlags _flags, bool _enabled)
    {
        super::SetInstanceFlags(_flags, _enabled);
    }

    //--------------------------------------------------------------------------------------
    UpdateFlags GameObject::GetUpdateFlags() const
    {
        return getUpdateFlags();
    }


    //--------------------------------------------------------------------------------------
    void GameObject::EnableTags(Tag _tags, bool _enabled)
    {
        if (_enabled)
            m_tags |= _tags;
        else
            m_tags &= ~_tags;
    }

    //--------------------------------------------------------------------------------------
    Tag GameObject::GetTags() const
    {
        return m_tags;
    }

    //--------------------------------------------------------------------------------------
    void GameObject::EnableUpdateFlags(UpdateFlags _flags, bool _enabled)
    {
        setUpdateFlags(_flags, _enabled);
    }

    //--------------------------------------------------------------------------------------
    IBaseScene * GameObject::GetScene() const
    {
        auto parent = GetParent();
        if (IBaseScene * scene = dynamic_cast<IBaseScene *>(parent))
            return scene;
        else if (IGameObject * go = dynamic_cast<IGameObject *>(parent))
            return go->GetScene();

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    IWorld * GameObject::GetWorld() const
    {
        if (auto scene = GetScene())
            return scene->GetWorld();

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::IsPrefab() const
    {
        VG_ASSERT(!asBool(ObjectFlags::Prefab & getObjectFlags()));
        return false;
    }

    //--------------------------------------------------------------------------------------
    IGameObject * GameObject::GetParentPrefab() const
    {
        //if (auto * parentScene = dynamic_cast<core::IBaseScene *>(getParent()))
        //{
        //    if (parentScene->GetSceneType() == BaseSceneType::Prefab)
        //        return parentScene->GetRoot();
        //}

        IGameObject * parent = dynamic_cast<IGameObject *>(GetParent());
        while (nullptr != parent)
        {
            if (parent->IsPrefab())
                return parent;

            //if (auto * parentScene = dynamic_cast<core::IBaseScene *>(parent->getParent()))
            //{
            //    if (parentScene->GetSceneType() == BaseSceneType::Prefab)
            //        return parentScene->GetRoot();
            //}
            
            if (auto * parentGO = dynamic_cast<IGameObject *>(parent->GetParent()))
                parent = parentGO;
            else
                parent = nullptr;
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    IDynamicPropertyList * GameObject::GetDynamicPropertyList(const IObject * _object) const
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    IDynamicPropertyList * GameObject::CreateDynamicPropertyList(const IObject * _object)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    IResource * GameObject::GetPrefabResource() const
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    IDynamicProperty * GameObject::GetDynamicProperty(const IObject * _object, const IProperty * _prop) const
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::CanOverrideProperty(const core::IObject * _object, const core::IProperty * _prop) const
    {
        return false;
    }

    //--------------------------------------------------------------------------------------
    IDynamicProperty * GameObject::CreateDynamicProperty(const core::IObject * _object, const core::IProperty * _prop)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OverrideGameObjectProperties(IGameObject * _gameObject, const IDynamicProperty * _dynProp)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::ToggleOverride(const IObject * _object, const IProperty * _prop, bool _override)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return false;
    }

    //--------------------------------------------------------------------------------------
    void GameObject::FixedUpdate(const Context & _context)
    {
        if (asBool(InstanceFlags::Enabled & getInstanceFlags()))
        {
            VG_PROFILE_CPU(GetName().c_str());
            VG_ASSERT(asBool(UpdateFlags::FixedUpdate & getUpdateFlags()), "[FixedUpdate] GameObject \"%s\" does not have the '%s' flag", GetName().c_str(), asString(UpdateFlags::FixedUpdate).c_str());

            ComponentUpdateContext componentUpdateContext(_context, this);

            for (uint i = 0; i < m_components.size(); ++i)
            {
                Component* component = m_components[i];
                if (asBool(ComponentFlags::Enabled & component->getComponentFlags()) && asBool(UpdateFlags::FixedUpdate & component->getUpdateFlags()))
                {
                    VG_PROFILE_CPU(component->GetClassName());
                    component->FixedUpdate(componentUpdateContext);
                }
            }

            const auto& children = getChildren();
            for (uint e = 0; e < children.size(); ++e)
            {
                auto* child = children[e];
                if (asBool(InstanceFlags::Enabled & child->getInstanceFlags()) && asBool(UpdateFlags::FixedUpdate & child->getUpdateFlags()))
                    child->FixedUpdate(componentUpdateContext);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::Update(const Context & _context)
    {
        if (asBool(InstanceFlags::Enabled & getInstanceFlags()))
        {
            VG_PROFILE_CPU(GetName().c_str());
            VG_ASSERT(asBool(UpdateFlags::Update & getUpdateFlags()), "[FixedUpdate] GameObject \"%s\" does not have the '%s' flag", GetName().c_str(), asString(UpdateFlags::Update).c_str());

            ComponentUpdateContext componentUpdateContext(_context, this);

            for (uint i = 0; i < m_components.size(); ++i)
            {
                Component* component = m_components[i];
                if (asBool(ComponentFlags::Enabled & component->getComponentFlags()) && asBool(UpdateFlags::Update & component->getUpdateFlags()))
                {
                    VG_PROFILE_CPU(component->GetClassName());
                    component->Update(componentUpdateContext);
                }
            }

            const auto& children = getChildren();
            for (uint e = 0; e < children.size(); ++e)
            {
                auto* child = children[e];
                if (asBool(InstanceFlags::Enabled & child->getInstanceFlags()) && asBool(UpdateFlags::Update & child->getUpdateFlags()))
                    child->Update(componentUpdateContext);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::LateUpdate(const Context & _context)
    {
        if (asBool(InstanceFlags::Enabled & getInstanceFlags()))
        {
            VG_PROFILE_CPU(GetName().c_str());
            VG_ASSERT(asBool(UpdateFlags::LateUpdate & getUpdateFlags()), "[FixedUpdate] GameObject \"%s\" does not have the '%s' flag", GetName().c_str(), asString(UpdateFlags::LateUpdate).c_str());

            ComponentUpdateContext componentUpdateContext(_context, this);

            for (uint i = 0; i < m_components.size(); ++i)
            {
                Component* component = m_components[i];
                if (asBool(ComponentFlags::Enabled & component->getComponentFlags()) && asBool(UpdateFlags::LateUpdate & component->getUpdateFlags()))
                {
                    VG_PROFILE_CPU(component->GetClassName());
                    component->LateUpdate(componentUpdateContext);
                }
            }

            const auto& children = getChildren();
            for (uint e = 0; e < children.size(); ++e)
            {
                auto* child = children[e];
                if (asBool(InstanceFlags::Enabled & child->getInstanceFlags()) && asBool(UpdateFlags::LateUpdate & child->getUpdateFlags()))
                    child->LateUpdate(componentUpdateContext);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::AddComponent(IComponent * _component, core::uint _index)
    {
        addComponent((Component*)_component, _index);
    }

    //--------------------------------------------------------------------------------------
    core::uint GameObject::GetComponentIndex(const IComponent * _component) const
    {
        uint componentIndex = -1;
        for (uint i = 0; i < m_components.size(); ++i)
        {
            if (m_components[i] == _component)
            {
                componentIndex = i;
                break;
            }
        }

        VG_ASSERT(-1 != componentIndex, "[GameObject] Component \"%s\" does not belong to GameObject \"%s\"", _component->GetName().c_str(), GetName().c_str());
        return componentIndex;
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::RemoveComponent(IComponent * _component)
    {
        if (vector_helper::exists(m_components, (Component *)_component))
        {
            vector_helper::remove(m_components, (Component *)_component);
            recomputeUpdateFlags();
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    const vector<IComponent *> & GameObject::GetComponents() const
    {
        return (vector<IComponent *> &)getComponents();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::addComponent(Component * _component, core::uint _index)
    {
        VG_ASSERT(_component);
        if (nullptr != _component)
        {
            VG_ASSERT(_index != -1 || m_components.end() == std::find(m_components.begin(), m_components.end(), _component));
            VG_SAFE_INCREASE_REFCOUNT(_component);

            if (-1 == _index || _index >= m_components.size())
                m_components.push_back(_component);
            else
                m_components.insert(m_components.begin() + _index, _component);

            _component->SetParent(this); 

            if (!_component->HasValidUID())
                _component->RegisterUID();

            recomputeUpdateFlags();
            sortComponents();
        }
    }

    //--------------------------------------------------------------------------------------
    const vector<Component*> & GameObject::getComponents() const
    {
        return m_components;
    }

    //--------------------------------------------------------------------------------------
    IGameObject * GameObject::GetChildGameObject(const string & _name) const 
    {
        if (GetName() == _name)
            return (IGameObject*)this;

        const auto & children = getChildren();
        for (uint i = 0; i < children.size(); ++i)
        {
            if (auto * child = children[i]->GetChildGameObject(_name))
                return child;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool IsA(IObject * _object, const char * _className)
    {
        const char * objectClassName = _object->GetClassName();
        if (!strcmp(objectClassName, _className))
            return true;

        const auto * classDesc = Kernel::getFactory()->GetClassDescriptor(objectClassName);
        if (nullptr != classDesc)
        {
            const char * parentClassName = nullptr;
            do
            {
                parentClassName = classDesc->GetParentClassName();

                if (!strcmp(parentClassName, _className))
                    return true;

                classDesc = Kernel::getFactory()->GetClassDescriptor(parentClassName, false);
            } while (nullptr != classDesc);
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    vector<IComponent *> GameObject::GetComponentsByType(const char * _className, bool _searchInParent, bool _searchInChildren) const
    {
        if (_searchInParent)
            VG_ASSERT_NOT_IMPLEMENTED();

        vector<IComponent *> found;

        const auto & components = getComponents();
        for (uint i = 0; i < components.size(); ++i)
        {
            auto * component = components[i];
            if (nullptr != component)
            {
                if (IsA(component, _className))
                    found.push_back(component);                
            }
        }

        if (_searchInChildren)
        {
            auto & children = getChildren();
            for (auto i = 0; i < children.size(); ++i)
            {
                // TODO: do this more efficiently using a separate method that takes R/W vector ref as paremeters
                auto others = children[i]->GetComponentsByType(_className, _searchInParent, _searchInChildren);

                if (others.size() > 0)
                    found.insert(found.end(), others.begin(), others.end());
            }
        }

        return found;
    }

    //--------------------------------------------------------------------------------------
    IComponent * GameObject::GetComponentByType(const char * _className, bool _searchInParent, bool _searchInChildren) const
    {
        auto * factory = Kernel::getFactory();
        const auto & components = getComponents();
        for (uint i = 0; i < components.size(); ++i)
        {
            auto * component = components[i];
            if (nullptr != component)
            {
                const char * componentClassName = component->GetClassName();
                if (!strcmp(componentClassName, _className))
                {
                    return component;
                }
                else 
                {
                    const auto * classDesc = factory->GetClassDescriptor(componentClassName, false);
                    if (nullptr != classDesc)
                    {
                        const char * interfaceName = classDesc->GetParentClassName();
                        while (nullptr != interfaceName)
                        {
                            if (!strcmp(interfaceName, _className))
                                return component;

                            classDesc = factory->GetClassDescriptor(interfaceName, false);
                            if (classDesc)
                                interfaceName = classDesc->GetParentClassName();
                            else
                                interfaceName = nullptr;
                        }
                    }
                }
            }
        }

        if (_searchInChildren)
        {
            auto & children = getChildren();
            for (uint i = 0; i < children.size(); ++i)
            {
                if (IComponent * component = children[i]->GetComponentByType(_className, _searchInParent, _searchInChildren))
                    return component;
            }
        }
        else if (_searchInParent)
        {
            if (auto * parent = dynamic_cast<IGameObject *>(GetParent()))
                return parent->GetComponentByType(_className, _searchInParent, _searchInChildren);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    IComponent * GameObject::AddComponent(const char * _className, const core::string & _name)
    {
        Component * component = (Component *)Kernel::getFactory()->CreateObject(_className, _name, this);
        if (component)
        {
            addComponent(component);
            component->Release();
        }
        return component;
    }

    //--------------------------------------------------------------------------------------
    void GameObject::AddChild(IGameObject * _gameObject, core::uint _index)
    {
        VG_ASSERT(_gameObject);
        if (nullptr != _gameObject)
        {
            VG_ASSERT(_index != -1 || m_children.end() == std::find(m_children.begin(), m_children.end(), _gameObject));
            VG_SAFE_INCREASE_REFCOUNT(_gameObject);

            if (-1 == _index || _index >= m_children.size())
                m_children.push_back((GameObject *)_gameObject);
            else
                m_children.insert(m_children.begin() + _index, (GameObject *)_gameObject);

            _gameObject->SetParent(this); // will recomputeUpdateFlags
        }
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::RemoveChild(IGameObject * _gameObject, bool _recomputeFlags)
    {
        if (vector_helper::exists(m_children, (GameObject*)_gameObject))
        {
            vector_helper::remove(m_children, (GameObject*)_gameObject);
            VG_SAFE_RELEASE(_gameObject);
            if (_recomputeFlags)
                recomputeUpdateFlags();
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    core::uint GameObject::RemoveAllChildren(bool _recomputeFlags)
    {
        if (m_children.size() > 0)
        {
            const auto count = m_children.size();
            for (uint i = 0; i < count; ++i)
                VG_SAFE_RELEASE(m_children[i]);
            m_children.clear();

            if (_recomputeFlags)
                recomputeUpdateFlags();
            return (core::uint)count;
        }
        return 0;
    }

    //--------------------------------------------------------------------------------------
    core::uint GameObject::GetChildIndex(const IGameObject * _child) const
    {
        uint childIndex = -1;
        for (uint i = 0; i < m_children.size(); ++i)
        {
            if (m_children[i] == _child)
            {
                childIndex = i;
                break;
            }
        }
        VG_ASSERT(-1 != childIndex, "[GameObject] GameObject \"%s\" is not a child of GameObject \"%s\"", _child->GetName().c_str(), GetName().c_str());
        return childIndex;
    }

    //--------------------------------------------------------------------------------------
    const vector<IGameObject*> & GameObject::GetChildren() const
    {
        return (const vector<IGameObject*> &)getChildren();
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::IsEnabledInHierarchy() const
    {
        const Instance * instance = this;

        while (instance)
        {
            if (!asBool(instance->GetInstanceFlags()))
                return false;
            instance = dynamic_cast<Instance *>(instance->GetParent());
        }

        return true;      
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::TryGetAABB(core::AABB & _aabb) const
    {
        AABB gameObjectAABB(float3(-0.01f, -0.01f, -0.01f), float3(-0.01f, -0.01f, -0.01f));

        const auto & components = getComponents();
        for (uint i = 0; i < components.size(); ++i)
        {
            auto * component = components[i];

            AABB componentAABB;
            if (component->TryGetAABB(componentAABB))
                gameObjectAABB.grow(componentAABB);
        }

        const auto & children = getChildren();
        for (uint i = 0; i < children.size(); ++i)
        {
            auto * child = children[i];

            AABB childAABB;
            if (child->TryGetAABB(childAABB))
            {
                float4x4 toParentMatrix = mul(child->getGlobalMatrix(), inverse(GetGlobalMatrix()));
                auto localSpaceChildAABB = AABB::transform(childAABB, toParentMatrix);
                gameObjectAABB.grow(localSpaceChildAABB);
            }
        }

        _aabb = gameObjectAABB;
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::IsRoot() const
    {
        return isRoot();
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::HasAncestor(const IGameObject * _ancestor) const
    {
        GameObject * parent = dynamic_cast<GameObject*>(GetParent());
        while (nullptr != parent)
        {
            if (parent == _ancestor)
                return true;
            if (nullptr != parent)
                parent = dynamic_cast<GameObject *>(parent->GetParent());
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        // Notify components that a GameObject property changed, but we don't want infinite loop when the property will update its parent
        for (uint i = 0; i < m_components.size(); ++i)
        {
            Component * component = m_components[i];
            if (nullptr != component)
                component->OnPropertyChanged(_object, _prop, false);
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::addGraphicInstance(renderer::IGraphicInstance * _graphicInstance)
    {
        VG_ASSERT(m_graphicInstances.end() == std::find(m_graphicInstances.begin(), m_graphicInstances.end(), _graphicInstance));
        _graphicInstance->AddRef();
        m_graphicInstances.push_back(_graphicInstance);
    }

    //--------------------------------------------------------------------------------------
    void GameObject::removeGraphicInstance(renderer::IGraphicInstance * _graphicInstance)
    {
        if (vector_helper::remove(m_graphicInstances, _graphicInstance))
            VG_SAFE_RELEASE(_graphicInstance);
    }

    //--------------------------------------------------------------------------------------
    const vector<renderer::IGraphicInstance*> & GameObject::getGraphicInstances() const
    {
        return m_graphicInstances;
    }

    //--------------------------------------------------------------------------------------
    void GameObject::recomputeUpdateFlags()
    {
        UpdateFlags update = (UpdateFlags)0x0;

        for (uint i = 0; i < m_children.size(); ++i)
        {
            if (auto * child = m_children[i])
                update |= child->getUpdateFlags();
        }

        for (uint i = 0; i < m_components.size(); ++i)
            update |= m_components[i]->getUpdateFlags();
        
        m_update = update;

        GameObject * parent = dynamic_cast<GameObject *>(GetParent());
        if (parent)
            parent->recomputeUpdateFlags();
    }

    //--------------------------------------------------------------------------------------
    // TODO: keep list of GameObject behaviours to avoid lookup
    //--------------------------------------------------------------------------------------
    const core::vector<IBehaviour *> /*&*/ GameObject::getBehaviours() const
    {
        return GetComponentsT<IBehaviour>();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnCollisionEnter(IGameObject * _other)
    {
        //VG_INFO("[Physics] OnCollisionEnter \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

        const auto & behaviours = getBehaviours();
        for (uint i = 0; i < behaviours.size(); ++i)
        {
            IBehaviour * behaviour = behaviours[i];
            behaviour->OnCollisionEnter(_other);
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnCollisionStay(IGameObject * _other)
    {
        //VG_INFO("[Physics] OnCollisionStay \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

        const auto & behaviours = getBehaviours();
        for (uint i = 0; i < behaviours.size(); ++i)
        {
            IBehaviour * behaviour = behaviours[i];
            behaviour->OnCollisionStay(_other);
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnCollisionExit(IGameObject * _other)
    {
        //VG_INFO("[Physics] OnCollisionExit \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

        const auto & behaviours = getBehaviours();
        for (uint i = 0; i < behaviours.size(); ++i)
        {
            IBehaviour * behaviour = behaviours[i];
            behaviour->OnCollisionExit(_other);
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnTriggerEnter(IGameObject * _other)
    {
        //VG_INFO("[Physics] OnTriggerEnter \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

        const auto & behaviours = getBehaviours();
        for (uint i = 0; i < behaviours.size(); ++i)
        {
            IBehaviour * behaviour = behaviours[i];
            behaviour->OnTriggerEnter(_other);
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnTriggerStay(IGameObject * _other)
    {
        //VG_INFO("[Physics] OnTriggerStay \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

        const auto & behaviours = getBehaviours();
        for (uint i = 0; i < behaviours.size(); ++i)
        {
            IBehaviour * behaviour = behaviours[i];
            behaviour->OnTriggerStay(_other);
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnTriggerExit(IGameObject * _other)
    {
        //VG_INFO("[Physics] OnTriggerExit \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

        const auto & behaviours = getBehaviours();
        for (uint i = 0; i < behaviours.size(); ++i)
        {
            IBehaviour * behaviour = behaviours[i];
            behaviour->OnTriggerExit(_other);
        }
    }
}