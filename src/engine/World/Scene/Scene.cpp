#include "engine/Precomp.h"
#include "Scene.h"
#include "core/GameObject/GameObject.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Kernel.h"

#if !VG_ENABLE_INLINE
#include "Scene.inl"
#endif

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS_EX(Scene, "Scene", IClassDesc::Flags::SceneNode);

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
    bool Scene::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectPtr(Scene, m_root, "Root");

        return true;
    }

    //--------------------------------------------------------------------------------------
    void Scene::SetRoot(IGameObject * _sector)
    {
        setRoot((GameObject*)_sector);
    }   

    //--------------------------------------------------------------------------------------
    void Scene::setRoot(GameObject * _sector)
    {
        if (_sector != m_root)
        {
            VG_SAFE_INCREASE_REFCOUNT(_sector);
            VG_SAFE_RELEASE(m_root);
            m_root = _sector;
        }
    }

    //--------------------------------------------------------------------------------------
    IGameObject * Scene::GetRoot() const
    {
        return (IGameObject*)getRoot();
    }
}