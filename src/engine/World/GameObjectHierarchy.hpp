#include "GameObjectHierarchy.h"
#include "core/GameObject/GameObject.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Kernel.h"
#include "engine/Resource/World/WorldResourceData.h"
#include "renderer/IGraphicScene.h"

using namespace vg::core;

namespace vg::engine
{
    //VG_REGISTER_CLASS_EX(GameObjectTree, "GameObjectTree", ClassDescFlags::SceneNode);

    //--------------------------------------------------------------------------------------
    GameObjectHierarchy::GameObjectHierarchy(const string & _name, IObject * _parent) :
        IBaseScene()
    {
        m_sceneRenderData = Engine::get()->GetRenderer()->CreateSceneRenderData(_name, this);
    }

    //--------------------------------------------------------------------------------------
    GameObjectHierarchy::~GameObjectHierarchy()
    {
        VG_SAFE_RELEASE(m_sceneRenderData);
        VG_SAFE_RELEASE(m_root);
    }

    //--------------------------------------------------------------------------------------
    core::IBaseSceneRenderData * GameObjectHierarchy::GetSceneRenderData() const
    {
        return m_sceneRenderData;
    }

    //--------------------------------------------------------------------------------------
    core::u32 GameObjectHierarchy::AddRef()
    {
        return super::AddRef();
    }

    //--------------------------------------------------------------------------------------
    core::u32 GameObjectHierarchy::Release()
    {
        return super::Release();
    }

    //--------------------------------------------------------------------------------------
    bool GameObjectHierarchy::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectPtrEx(Scene, m_root, "Root", core::PropertyFlags::Hidden);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void GameObjectHierarchy::SetRoot(IGameObject * _sector)
    {
        setRoot((GameObject *)_sector);
    }

    //--------------------------------------------------------------------------------------
    void GameObjectHierarchy::setRoot(GameObject * _sector)
    {
        if (_sector != m_root)
        {
            VG_SAFE_INCREASE_REFCOUNT(_sector);
            VG_SAFE_RELEASE(m_root);
            m_root = _sector;
        }
    }

    //--------------------------------------------------------------------------------------
    IGameObject * GameObjectHierarchy::GetRoot() const
    {
        return (IGameObject *)getRoot();
    }

    //--------------------------------------------------------------------------------------
    core::IWorld * GameObjectHierarchy::GetWorld() const
    {
        auto parent = GetParent();
        while (parent)
        {
            if (auto world = dynamic_cast<IWorld *>(parent))
                return world;

            parent = parent->GetParent();
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void GameObjectHierarchy::OnResourceLoaded(core::IResource * _resource)
    {

    }

    //--------------------------------------------------------------------------------------
    void GameObjectHierarchy::OnResourceUnloaded(core::IResource * _resource)
    {

    }
}