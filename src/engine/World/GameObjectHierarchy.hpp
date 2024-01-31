#include "GameObjectHierarchy.h"
#include "core/GameObject/GameObject.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Kernel.h"

using namespace vg::core;

namespace vg::engine
{
    //VG_REGISTER_OBJECT_CLASS_EX(GameObjectTree, "GameObjectTree", IClassDesc::Flags::SceneNode);

    //--------------------------------------------------------------------------------------
    GameObjectHierarchy::GameObjectHierarchy(const string & _name, IObject * _parent) :
        IBaseScene()
    {

    }

    //--------------------------------------------------------------------------------------
    GameObjectHierarchy::~GameObjectHierarchy()
    {
        VG_SAFE_RELEASE(m_root);
    }

    //--------------------------------------------------------------------------------------
    bool GameObjectHierarchy::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectPtr(Scene, m_root, "Root");

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
}