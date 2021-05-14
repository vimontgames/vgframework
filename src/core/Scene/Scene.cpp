#include "core/Precomp.h"
#include "Scene.h"
#include "core/Sector/Sector.h"
#include "core/Object/AutoRegisterClass.h"

namespace vg::core
{
    VG_AUTO_REGISTER_CLASS(Scene);

    //--------------------------------------------------------------------------------------
    Scene::Scene(const string & _name, IObject * _parent) :
        IScene()
    {

    }

    //--------------------------------------------------------------------------------------
    Scene::~Scene()
    {
        VG_SAFE_RELEASE(m_root);
    }

    //--------------------------------------------------------------------------------------
    bool Scene::registerClass(IObjectFactory & _factory)
    {
        if (core::IObjectDescriptor * desc = _factory.registerClassHelper(Scene, "Scene", IObjectDescriptor::Flags::SceneNode))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Scene::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerProperty("m_root", (IObject**)(&((Scene*)(nullptr))->m_root), "Root", IPropertyDescriptor::Flags::None);

        _desc.registerCallbackHelper(Scene, load, "Load", IPropertyDescriptor::Flags::None);
        _desc.registerCallbackHelper(Scene, save, "Save", IPropertyDescriptor::Flags::SameLine);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Scene::load(IObject * _object)
    {
        return _object->loadFromFile(_object->getName() + ".scene");
    }

    //--------------------------------------------------------------------------------------
    bool Scene::save(IObject * _object)
    {
        return _object->saveToFile(_object->getName() + ".scene");
    }

    //--------------------------------------------------------------------------------------
    void Scene::setRoot(ISector * _sector)
    {
        if (_sector != m_root)
        {
            VG_SAFE_INCREASE_REFCOUNT(_sector);
            VG_SAFE_RELEASE(m_root);
            m_root = _sector;
        }
    }

    //--------------------------------------------------------------------------------------
    ISector * Scene::getRoot() const
    {
        return m_root;
    }
}