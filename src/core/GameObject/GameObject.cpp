#include "core/Precomp.h"
#include "GameObject.h"
#include "core/Kernel.h"
#include "core/Component/Component.h"
#include "core/Object/AutoRegisterClass.h"  
#include "renderer/IGraphicInstance.h"

namespace vg::core
{
    VG_AUTO_REGISTER_CLASS(GameObject);

    //--------------------------------------------------------------------------------------
    bool GameObject::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(GameObject, "GameObject", IClassDesc::Flags::GameObject))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.getPropertyByName("m_name")->setFlags(IProperty::Flags::None, IProperty::Flags::Hidden);
        _desc.registerPropertyObjectRefVectorHelper(GameObject, m_components, "Components", IProperty::Flags::None);
        _desc.registerPropertyObjectRefVectorHelper(GameObject, m_children, "Children", IProperty::Flags::Hidden);

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
    void GameObject::Update(double _dt)
    {
        if (GameObject::Flags::Enabled & getFlags())
        {
            for (uint i = 0; i < m_components.size(); ++i)
            {
                Component * component = m_components[i];
                if (Component::Flags::Enabled & component->getFlags())
                    component->Update(_dt);
            }

            const auto & children = getChildren();
            for (uint e = 0; e < children.size(); ++e)
            {
                auto * child = children[e];
                child->Update(_dt);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void GameObject::AddComponent(IComponent * _component)
    {
        addComponent((Component*)_component);
    }
    //--------------------------------------------------------------------------------------
    const vector<IComponent *> & GameObject::GetComponents() const
    {
        return (vector<IComponent *> &)getComponents();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::addComponent(Component * _component)
    {
        VG_ASSERT(_component);
        _component->addRef();
        m_components.push_back(_component);
    }

    //--------------------------------------------------------------------------------------
    const vector<Component*> & GameObject::getComponents() const
    {
        return m_components;
    }

    //--------------------------------------------------------------------------------------
    Component * GameObject::findComponentByType(const char * _className) const
    {
        const auto & components = getComponents();
        for (uint i = 0; i < components.size(); ++i)
        {
            auto * component = components[i];
            if (component && !strcmp(component->getClassName(), _className)) // TODO: RTTI/inheritance
                return component;
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    Component * GameObject::addComponent(const char * _className, const core::string & _name)
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
    void GameObject::AddChild(IGameObject * _gameObject)
    {
        VG_ASSERT(m_children.end() == std::find(m_children.begin(), m_children.end(), _gameObject));
        VG_SAFE_INCREASE_REFCOUNT(_gameObject);
        m_children.push_back((GameObject*)_gameObject);
        _gameObject->setParent(this);
    }

    //--------------------------------------------------------------------------------------
    bool GameObject::RemoveChild(IGameObject * _gameObject)
    {
        if (m_children.exists((GameObject*)_gameObject))
        {
            m_children.remove((GameObject*)_gameObject);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    const vector<IGameObject*> & GameObject::GetChildren() const
    {
        return (const vector<IGameObject*> &)getChildren();
    }

    //--------------------------------------------------------------------------------------
    void GameObject::AddGraphicInstance(renderer::IGraphicInstance * _graphicInstance)
    {
        VG_ASSERT(m_graphicInstances.end() == std::find(m_graphicInstances.begin(), m_graphicInstances.end(), _graphicInstance));
        _graphicInstance->addRef();
        m_graphicInstances.push_back(_graphicInstance);
    }

    //--------------------------------------------------------------------------------------
    void GameObject::RemoveGraphicInstance(renderer::IGraphicInstance * _graphicInstance)
    {
        if (m_graphicInstances.remove(_graphicInstance))
            VG_SAFE_RELEASE(_graphicInstance);
    }

    //--------------------------------------------------------------------------------------
    const vector<renderer::IGraphicInstance*> & GameObject::GetGraphicInstances() const
    {
        return m_graphicInstances;
    }
}