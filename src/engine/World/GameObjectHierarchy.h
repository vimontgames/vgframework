#pragma once

#include "core/IBaseScene.h"

namespace vg::core
{
    class IGameObject;
    class GameObject;
}

namespace vg::engine
{

    class GameObjectHierarchy : public core::IBaseScene
    {
    public:
        VG_CLASS_DECL(GameObjectHierarchy, core::IBaseScene);

                                        GameObjectHierarchy      (const core::string & _name, core::IObject * _parent);
                                        ~GameObjectHierarchy     ();

        core::u32						AddRef                  () override;
        core::u32						Release                 () override;

        void                            SetRoot                 (core::IGameObject * _sector) final override;
        core::IGameObject *             GetRoot                 () const final override;

        core::IWorld *                  GetWorld                () const final override;

        void                            OnResourceLoaded        (core::IResource * _resource) final override;
        void                            OnResourceUnloaded      (core::IResource * _resource) final override;

    public:
        void                            setRoot                 (core::GameObject * _sector);
        VG_INLINE core::GameObject *    getRoot                 () const { return m_root; }

    private:
        core::GameObject *              m_root                  = nullptr;
    };
}