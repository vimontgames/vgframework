#include "core/Precomp.h"
#include "GameObject.h"
#include "core/Kernel.h"
#include "core/Component/Component.h"
#include "core/Object/AutoRegisterClass.h"  
#include "core/ISelection.h"
#include "core/IProfiler.h"
#include "renderer/IGraphicInstance.h"
#include "core/IBaseScene.h"

#if !VG_ENABLE_INLINE
#include "GameObject.inl"
#endif

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS_EX(GameObject, "GameObject", IClassDesc::Flags::GameObject);

    //--------------------------------------------------------------------------------------
    bool GameObject::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        // Hide GameObject name
        _desc.GetPropertyByName("m_name")->setFlags(IProperty::Flags::None, IProperty::Flags::Hidden);

        registerPropertyObjectPtrVector(GameObject, m_components, "Components");
        registerPropertyObjectPtrVectorEx(GameObject, m_children, "Children", IProperty::Flags::Hidden);

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
        if (nullptr != selection)
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
        recomputeUpdateFlags();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnPlay()
    {
        super::OnPlay();

        for (uint i = 0; i < m_components.size(); ++i)
            m_components[i]->OnPlay();

        for (uint i = 0; i < m_children.size(); ++i)
            m_children[i]->OnPlay();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::OnStop()
    {
        for (uint i = 0; i < m_children.size(); ++i)
            m_children[i]->OnStop();

        for (uint i = 0; i < m_components.size(); ++i)
            m_components[i]->OnStop();

        super::OnStop();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::setParent(IObject * _parent)
    {
        super::setParent(_parent);
        recomputeUpdateFlags();
    }

    //--------------------------------------------------------------------------------------
    UpdateFlags GameObject::GetUpdateFlags() const
    {
        return getUpdateFlags();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::SetUpdateFlags(UpdateFlags _flags, bool _enabled)
    {
        setUpdateFlags(_flags, _enabled);
    }

    //--------------------------------------------------------------------------------------
    IBaseScene * GameObject::GetScene() const
    {
        auto parent = getParent();
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
        return false;
    }

    //--------------------------------------------------------------------------------------
    IResource * GameObject::GetPrefabResource() const
    {
        VG_ASSERT(IsPrefab());
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void GameObject::FixedUpdate(float _dt)
    {
        if (asBool(InstanceFlags::Enabled & getInstanceFlags()))
        {
            VG_PROFILE_CPU(getName().c_str());
            VG_ASSERT(asBool(UpdateFlags::FixedUpdate & getUpdateFlags()), "[FixedUpdate] GameObject \"%s\" does not have the '%s' flag", getName().c_str(), asString(UpdateFlags::FixedUpdate).c_str());

            for (uint i = 0; i < m_components.size(); ++i)
            {
                Component* component = m_components[i];
                if (asBool(ComponentFlags::Enabled & component->getComponentFlags()) && asBool(UpdateFlags::FixedUpdate & component->getUpdateFlags()))
                    component->FixedUpdate(_dt);
            }

            const auto& children = getChildren();
            for (uint e = 0; e < children.size(); ++e)
            {
                auto* child = children[e];
                if (asBool(InstanceFlags::Enabled & child->getInstanceFlags()) && asBool(UpdateFlags::FixedUpdate & child->getUpdateFlags()))
                    child->FixedUpdate(_dt);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::Update(float _dt)
    {
        if (asBool(InstanceFlags::Enabled & getInstanceFlags()))
        {
            VG_PROFILE_CPU(getName().c_str());
            VG_ASSERT(asBool(UpdateFlags::Update & getUpdateFlags()), "[FixedUpdate] GameObject \"%s\" does not have the '%s' flag", getName().c_str(), asString(UpdateFlags::Update).c_str());

            for (uint i = 0; i < m_components.size(); ++i)
            {
                Component* component = m_components[i];
                if (asBool(ComponentFlags::Enabled & component->getComponentFlags()) && asBool(UpdateFlags::Update & component->getUpdateFlags()))
                    component->Update(_dt);
            }

            const auto& children = getChildren();
            for (uint e = 0; e < children.size(); ++e)
            {
                auto* child = children[e];
                if (asBool(InstanceFlags::Enabled & child->getInstanceFlags()) && asBool(UpdateFlags::Update & child->getUpdateFlags()))
                    child->Update(_dt);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::LateUpdate(float _dt)
    {
        if (asBool(InstanceFlags::Enabled & getInstanceFlags()))
        {
            VG_PROFILE_CPU(getName().c_str());
            VG_ASSERT(asBool(UpdateFlags::LateUpdate & getUpdateFlags()), "[FixedUpdate] GameObject \"%s\" does not have the '%s' flag", getName().c_str(), asString(UpdateFlags::LateUpdate).c_str());

            for (uint i = 0; i < m_components.size(); ++i)
            {
                Component* component = m_components[i];
                if (asBool(ComponentFlags::Enabled & component->getComponentFlags()) && asBool(UpdateFlags::LateUpdate & component->getUpdateFlags()))
                    component->LateUpdate(_dt);
            }

            const auto& children = getChildren();
            for (uint e = 0; e < children.size(); ++e)
            {
                auto* child = children[e];
                if (asBool(InstanceFlags::Enabled & child->getInstanceFlags()) && asBool(UpdateFlags::LateUpdate & child->getUpdateFlags()))
                    child->LateUpdate(_dt);
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

        VG_ASSERT(-1 != componentIndex, "[GameObject] Component \"%s\" does not belong to GameObject \"%s\"", _component->getName().c_str(), getName().c_str());
        return componentIndex;
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::RemoveComponent(IComponent * _component)
    {
        if (m_components.exists((Component *)_component))
        {
            m_components.remove((Component *)_component);
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

            _component->setParent(this); 
            recomputeUpdateFlags();
        }
    }

    //--------------------------------------------------------------------------------------
    const vector<Component*> & GameObject::getComponents() const
    {
        return m_components;
    }

    //--------------------------------------------------------------------------------------
    IComponent * GameObject::GetComponentByType(const char * _className) const
    {
        const auto & components = getComponents();
        for (uint i = 0; i < components.size(); ++i)
        {
            auto * component = components[i];
            if (nullptr != component)
            {
                if (!strcmp(component->getClassName(), _className))
                {
                    return component;
                }
                else 
                {
                    const auto * classDesc = Kernel::getFactory()->getClassDescriptor(component->getClassName());
                    if (nullptr != classDesc)
                    {
                        const char * interfaceName = classDesc->GetParentClassName();
                        if (nullptr != interfaceName)
                        {
                            if (!strcmp(interfaceName, _className))
                            {
                                return component;
                            }
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    IComponent * GameObject::AddComponent(const char * _className, const core::string & _name)
    {
        Component * component = (Component *)Kernel::getFactory()->createObject(_className, _name, this);
        if (component)
        {
            addComponent(component);
            component->release();
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

            _gameObject->setParent(this); // will recomputeUpdateFlags
        }
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::RemoveChild(IGameObject * _gameObject)
    {
        if (m_children.exists((GameObject*)_gameObject))
        {
            m_children.remove((GameObject*)_gameObject);
            VG_SAFE_RELEASE(_gameObject);
            recomputeUpdateFlags();
            return true;
        }

        return false;
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
        VG_ASSERT(-1 != childIndex, "[GameObject] GameObject \"%s\" is not a child of GameObject \"%s\"", _child->getName().c_str(), getName().c_str());
        return childIndex;
    }

    //--------------------------------------------------------------------------------------
    const vector<IGameObject*> & GameObject::GetChildren() const
    {
        return (const vector<IGameObject*> &)getChildren();
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::IsRoot() const
    {
        const auto parent = getParent();
        return nullptr == parent || nullptr == dynamic_cast<const GameObject *>(parent);
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::HasAncestor(const IGameObject * _ancestor) const
    {
        GameObject * parent = (GameObject*)getParent();
        while (nullptr != parent)
        {
            if (parent == _ancestor)
                return true;
            parent = (GameObject *)parent->getParent();
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
        _graphicInstance->addRef();
        m_graphicInstances.push_back(_graphicInstance);
    }

    //--------------------------------------------------------------------------------------
    void GameObject::removeGraphicInstance(renderer::IGraphicInstance * _graphicInstance)
    {
        if (m_graphicInstances.remove(_graphicInstance))
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

        GameObject * parent = dynamic_cast<GameObject *>(getParent());
        if (parent)
            parent->recomputeUpdateFlags();
    }
}