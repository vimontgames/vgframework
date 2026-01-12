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

#include "renderer/IGraphicScene.h"
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

        registerPropertyEnumBitfieldEx(GameObject, UpdateFlags, m_update, "Update", PropertyFlags::Transient | PropertyFlags::DebugRuntime);

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
        //OnDisable();

        // Remove GameObject from selection when deleted
        ISelection * selection = Kernel::getSelection(false);
        if (nullptr != selection && selection->IsSelectedObject(this))
            selection->Remove(this);

        for (uint i = 0; i < m_children.size(); ++i)
            VG_SAFE_RELEASE(m_children[i]);
        m_children.clear();

        for (uint i = 0; i < m_components.size(); ++i)
            VG_SAFE_RELEASE(m_components[i]);
        m_components.clear();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnLoad()
    {
        //VG_DEBUGPRINT("[GameObject] Load \"%s\"\n", GetName().c_str());
        super::OnLoad();
        RegisterUID();
        recomputeUpdateFlags();
        sortComponents();
        OnLocalMatrixChanged(true); // recompute parent matrix
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
        if (isEnabledInHierarchy())
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
        if (isEnabledInHierarchy())
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
        if (isEnabledInHierarchy())
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
        {
            auto * component = m_components[i];
            //if (component->isEnabled())
                m_components[i]->OnDisable();
        }

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
        recomputeEnabledInHierarchy();
        super::SetInstanceFlags(_flags, _enabled);
        recomputeUpdateFlags();
    }

    //--------------------------------------------------------------------------------------
    UpdateFlags GameObject::GetUpdateFlags() const
    {
        if (isEnabledInHierarchy())
            return getUpdateFlags();
        else
            return (UpdateFlags)0x0;
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
        VG_ASSERT(!asBool(ObjectRuntimeFlags::Prefab & getObjectRuntimeFlags()));
        return false;
    }

    //--------------------------------------------------------------------------------------
    IGameObject * GameObject::GetParentPrefab() const
    {
        IGameObject * parent = dynamic_cast<IGameObject *>(GetParent());
        while (nullptr != parent)
        {
            if (parent->IsPrefab())
            {
                if (auto * nestedPrefabParent = parent->GetParentPrefab())
                    return nestedPrefabParent;
                else
                    return parent;
            }

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
    bool GameObject::EnablePropertyOverride(const IObject * _object, const IProperty * _prop, bool _override)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::DestroyDynamicOverride(const IObject * _object, const IProperty * _prop)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        return false;
    }

    //--------------------------------------------------------------------------------------
    void GameObject::FixedUpdate(const Context & _context)
    {
        // Static object do not need update every frame 
        const InstanceFlags instanceFlags = getInstanceFlags();
        if (asBool(InstanceFlags::Static & instanceFlags))
            return;

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
        // Static object do not need update every frame 
        const InstanceFlags instanceFlags = getInstanceFlags();
        if (asBool(InstanceFlags::Static & instanceFlags))
            return;

        if (asBool(InstanceFlags::Enabled & instanceFlags))
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
        // Static object do not need update every frame 
        const InstanceFlags instanceFlags = getInstanceFlags();
        if (asBool(InstanceFlags::Static & instanceFlags))
            return;

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
    void GameObject::ToolUpdate(const Context & _context)
    {
        if (asBool(InstanceFlags::Enabled & getInstanceFlags()))
        {
            VG_PROFILE_CPU(GetName().c_str());
            VG_ASSERT(asBool(UpdateFlags::ToolUpdate & getUpdateFlags()), "[ToolUpdate] GameObject \"%s\" does not have the '%s' flag", GetName().c_str(), asString(UpdateFlags::LateUpdate).c_str());

            ComponentUpdateContext componentUpdateContext(_context, this);

            for (uint i = 0; i < m_components.size(); ++i)
            {
                Component * component = m_components[i];
                if (asBool(ComponentFlags::Enabled & component->getComponentFlags()) && asBool(UpdateFlags::ToolUpdate & component->getUpdateFlags()))
                {
                    VG_PROFILE_CPU(component->GetClassName());
                    component->ToolUpdate(componentUpdateContext);
                }
            }

            const auto & children = getChildren();
            for (uint e = 0; e < children.size(); ++e)
            {
                auto * child = children[e];
                if (asBool(InstanceFlags::Enabled & child->getInstanceFlags()) && asBool(UpdateFlags::ToolUpdate & child->getUpdateFlags()))
                    child->ToolUpdate(componentUpdateContext);
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
    vector<IComponent *> GameObject::GetComponentsByType(const char * _className, bool _searchInParent, bool _searchInChildren) const
    {
        if (_searchInParent)
            VG_ASSERT_NOT_IMPLEMENTED();

        const auto * factory = Kernel::getFactory();
        const ClassCRC classCRC = computeCRC64(_className);

        vector<IComponent *> found;

        const auto & components = getComponents();
        for (uint i = 0; i < components.size(); ++i)
        {
            auto * component = components[i];
            if (nullptr != component)
            {
                if (factory->IsA(component->GetClassCRC(), classCRC))
                    found.push_back(component);                
            }
        }

        if (_searchInChildren)
        {
            auto & children = getChildren();
            for (auto i = 0; i < children.size(); ++i)
            {
                // TODO: do this more efficiently using a separate method that takes R/W vector ref as parameters
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
        const ClassCRC classCRC = computeCRC64(_className);

        auto * factory = Kernel::getFactory();
        const auto & components = getComponents();
        for (uint i = 0; i < components.size(); ++i)
        {
            auto * component = components[i];
            if (nullptr != component)
            {
                const char * componentClassName = component->GetClassName();
                const ClassCRC componentClassCRC = component->GetClassCRC();

                if (classCRC == componentClassCRC)
                {
                    return component;
                }
                else 
                {
                    const auto * classDesc = factory->GetClassDescriptorFromCRC(componentClassCRC, false);
                    if (nullptr != classDesc)
                    {
                        const char * interfaceName = classDesc->GetParentClassName();
                        ClassCRC parentClassCRC = classDesc->GetParentClassCRC();

                        while (0 != parentClassCRC)
                        {
                            if (parentClassCRC == classCRC)
                                return component;

                            classDesc = factory->GetClassDescriptorFromCRC(parentClassCRC, false);
                            if (classDesc)
                                parentClassCRC = classDesc->GetParentClassCRC();
                            else
                                parentClassCRC = 0;
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
        //const Instance * instance = this;
        //
        //while (instance)
        //{
        //    if (!asBool(instance->GetInstanceFlags()))
        //        return false;
        //    instance = dynamic_cast<Instance *>(instance->GetParent());
        //}
        //
        //return true;    
        // 
        
        return isEnabledInHierarchy();
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
        super::OnPropertyChanged(_object, _prop, _notifyParent);

        // Recompute hierarchy update flags when Instance flags change
        if (!strcmp(_prop.GetName(), "m_flags"))
            recomputeUpdateFlags();

        // Notify components and children that a GameObject property changed, but we don't want infinite loop when the property will update its parent
        for (uint i = 0; i < m_components.size(); ++i)
        {
            Component * component = m_components[i];
            if (nullptr != component)
                component->OnPropertyChanged(_object, _prop, false);
        }

        for (uint i = 0; i < m_children.size(); ++i)
            m_children[i]->OnPropertyChanged(_object, _prop, false);
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::addGraphicInstance(renderer::IGraphicInstance * _graphicInstance)
    {
        if (auto * scene = GetScene())
        {
            if (auto * sceneRenderData = scene->GetSceneRenderData())
            {
                VG_ASSERT(VG_SAFE_STATIC_CAST(renderer::IGraphicScene, sceneRenderData));
                if (((renderer::IGraphicScene *)sceneRenderData)->RegisterGraphicInstance(_graphicInstance))
                    return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::removeGraphicInstance(renderer::IGraphicInstance * _graphicInstance)
    {
        if (nullptr != _graphicInstance)
        {
            if (auto * scene = GetScene())
            {
                if (auto * sceneRenderData = scene->GetSceneRenderData())
                {
                    VG_ASSERT(VG_SAFE_STATIC_CAST(renderer::IGraphicScene, sceneRenderData));
                    if (((renderer::IGraphicScene *)sceneRenderData)->UnregisterGraphicInstance(_graphicInstance))
                        return true;
                }
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void GameObject::recomputeEnabledInHierarchy()
    {
        bool parentIsEnabled = true;
        IGameObject * parent = dynamic_cast<IGameObject *>(GetParent());
        while (parent)
        {
            if (!parent->isLocalEnabled())
            {
                parentIsEnabled = false;
                break;
            }

            parent = dynamic_cast<IGameObject *>(parent->GetParent());
        }

        const bool enabled = parentIsEnabled && isLocalEnabled();

        if (enabled)
        {
            if (!asBool(InstanceRuntimeFlags::Active & GetInstanceRuntimeFlags()))
            {
                SetInstanceRuntimeFlags(InstanceRuntimeFlags::Active, true);

                for (uint i = 0; i < m_components.size(); ++i)
                {
                    Component * component = m_components[i];
                    if (component->isEnabled())
                        component->OnEnable();
                    component->UpdateFlagsFromGameObject();
                }
            }
        }
        else
        {
            if (asBool(InstanceRuntimeFlags::Active & GetInstanceRuntimeFlags()))
            {
                SetInstanceRuntimeFlags(InstanceRuntimeFlags::Active, false);

                for (uint i = 0; i < m_components.size(); ++i)
                {
                    Component * component = m_components[i];
                    component->OnDisable();
                    component->UpdateFlagsFromGameObject();
                }
            }
        }

        for (uint i = 0; i < m_children.size(); ++i)
        {
            if (auto * child = m_children[i])
                child->recomputeEnabledInHierarchy();
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::recomputeChildrenUpdateFlags(vector<GameObject *> & _leaves)
    {
        UpdateFlags flags = (UpdateFlags)0x0;

        // Compute self update flags
        for (uint i = 0; i < m_components.size(); ++i)
        {
            Component * component = m_components[i];
            flags |= component->getUpdateFlags();
        }

        m_update = flags;

        // Also recompute them for all children, or add to 'leaves' list
        if (m_children.size() > 0)
        {
            for (uint i = 0; i < m_children.size(); ++i)
            {
                if (auto * child = m_children[i])
                    child->recomputeChildrenUpdateFlags(_leaves);
            }
        }
        else
        {
            _leaves.push_back(this);
        }
    }

    //--------------------------------------------------------------------------------------
    // To compute GameObject's update flags, 'OR' the flags from all its descendants
    // A GameObject is also considered 'Static' if it has children and all its children are 'Static'.
    //--------------------------------------------------------------------------------------
    void GameObject::recomputeFlagsFromChildren()
    {
        bool allChildrenAreStatic = true;

        for (uint i = 0; i < m_children.size(); ++i)
        {
            if (auto * child = m_children[i])
            {
                m_update |= child->getUpdateFlags();

                if (!asBool(InstanceFlags::Static & child->getInstanceFlags()))
                    allChildrenAreStatic = false;
            }
        }

        if (m_children.size() > 0)
        {
            if (allChildrenAreStatic)
            {
                if (!asBool(InstanceFlags::Static & getInstanceFlags()))
                {
                    VG_WARNING("[GameObject] 'InstanceFlags::Static' has been set on GameObject \"%s\" because all its children are static", GetFullName().c_str());
                    setInstanceFlags(InstanceFlags::Static, true);
                }
            }
            else
            {
                if (asBool(InstanceFlags::Static & getInstanceFlags()))
                {
                    VG_WARNING("[GameObject] 'InstanceFlags::Static' has been removed from GameObject \"%s\" because at least one of its children is not static", GetFullName().c_str());
                    setInstanceFlags(InstanceFlags::Static, false);
                }
            }
        }

        GameObject * parentGO = dynamic_cast<GameObject *>(GetParent());
        if (parentGO)
            parentGO->recomputeFlagsFromChildren();
    }

    //--------------------------------------------------------------------------------------
    // Recompute children update flags and propagate to direct parent in two passes
    //--------------------------------------------------------------------------------------
    void GameObject::recomputeUpdateFlags()
    {
        // If GameObject is enabled then all its children could be enabled
        recomputeEnabledInHierarchy();

        vector<GameObject *> leaves;

        // 1. Refresh update flags from the GameObject components, and do the same for all its children (also adding leaves to list for parent propagation)
        recomputeChildrenUpdateFlags(leaves);

        // Propagate from leaves to parents (Parent will 'OR' the flags from all their children)
        for (uint i = 0; i < leaves.size(); ++i)
        {
            GameObject * go = leaves[i];
            go->recomputeFlagsFromChildren();
        }
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
        if (IsEnabledInHierarchy())
        {
            //VG_INFO("[Physics] OnCollisionEnter \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

            const auto & behaviours = getBehaviours();
            for (uint i = 0; i < behaviours.size(); ++i)
            {
                IBehaviour * behaviour = behaviours[i];
                if (behaviour->isEnabled())
                    behaviour->OnCollisionEnter(_other);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnCollisionStay(IGameObject * _other)
    {
        if (IsEnabledInHierarchy())
        {
            //VG_INFO("[Physics] OnCollisionStay \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

            const auto & behaviours = getBehaviours();
            for (uint i = 0; i < behaviours.size(); ++i)
            {
                IBehaviour * behaviour = behaviours[i];
                if (behaviour->isEnabled())
                    behaviour->OnCollisionStay(_other);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnCollisionExit(IGameObject * _other)
    {
        if (IsEnabledInHierarchy())
        {
            //VG_INFO("[Physics] OnCollisionExit \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

            const auto & behaviours = getBehaviours();
            for (uint i = 0; i < behaviours.size(); ++i)
            {
                IBehaviour * behaviour = behaviours[i];
                if (behaviour->isEnabled())
                    behaviour->OnCollisionExit(_other);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnTriggerEnter(IGameObject * _other)
    {
        if (IsEnabledInHierarchy())
        {
            //VG_INFO("[Physics] OnTriggerEnter \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

            const auto & behaviours = getBehaviours();
            for (uint i = 0; i < behaviours.size(); ++i)
            {
                IBehaviour * behaviour = behaviours[i];
                if (behaviour->isEnabled())
                    behaviour->OnTriggerEnter(_other);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnTriggerStay(IGameObject * _other)
    {
        if (IsEnabledInHierarchy())
        {
            if (IsEnabledInHierarchy())
            {
                //VG_INFO("[Physics] OnTriggerStay \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

                const auto & behaviours = getBehaviours();
                for (uint i = 0; i < behaviours.size(); ++i)
                {
                    IBehaviour * behaviour = behaviours[i];
                    if (behaviour->isEnabled())
                        behaviour->OnTriggerStay(_other);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnTriggerExit(IGameObject * _other)
    {
        if (IsEnabledInHierarchy())
        {
            //VG_INFO("[Physics] OnTriggerExit \"%s\" vs. \"%s\"", getName().c_str(), _other->getName().c_str());

            const auto & behaviours = getBehaviours();
            for (uint i = 0; i < behaviours.size(); ++i)
            {
                IBehaviour * behaviour = behaviours[i];
                if (behaviour->isEnabled())
                    behaviour->OnTriggerExit(_other);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnLocalMatrixChanged(bool _recomputeParents, bool _recomputeChildren)
    {
        super::OnLocalMatrixChanged(_recomputeParents, _recomputeChildren);

        if (_recomputeChildren)
        {
            VG_PROFILE_CPU("RecomputeChildrenLocalMatrix");

            for (uint i = 0; i < m_children.size(); ++i)
                m_children[i]->OnLocalMatrixChanged(false, true);
        }
    }
}