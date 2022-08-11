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
    bool Scene::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(Scene, "Scene", IClassDesc::Flags::SceneNode))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Scene::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerProperty("Scene", "m_root", (IObject**)(&((Scene*)(nullptr))->m_root), "Root", IProperty::Flags::None);

        _desc.registerCallbackHelper(Scene, load, "Load", IProperty::Flags::None);
        _desc.registerCallbackHelper(Scene, save, "Save", IProperty::Flags::SameLine);

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