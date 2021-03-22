#include "core/Precomp.h"
#include "Scene.h"
#include "core/Sector/Sector.h"
#include "core/Object/AutoRegisterClass.h"

namespace vg::core
{
    VG_AUTO_REGISTER_CLASS(Scene)

    //--------------------------------------------------------------------------------------
    Scene::Scene() :
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
        if (core::IObjectDescriptor * desc = _factory.registerClassHelper(Scene, "Scene"))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Scene::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerProperty("m_root", (IObject**)(&((Scene*)(nullptr))->m_root), "Root", IPropertyDescriptor::Flags::None);

        return true;
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