#pragma once

#include "core/IScene.h"

namespace vg::core
{
    class GameObject;
    class IFactory;
    class IClassDesc;

    class Scene : public IScene
    {
    public:
        VG_CLASS_DECL(Scene, IScene);

                                Scene               (const string & _name, IObject * _parent);
                                ~Scene              ();

        void                    SetRoot             (IGameObject * _sector) final;
        IGameObject *           GetRoot             () const final;

    public:
        void                    setRoot             (GameObject * _sector);
        VG_INLINE GameObject *  getRoot             () const;

    private:
        GameObject *            m_root              = nullptr;
    };
}

#if VG_ENABLE_INLINE
#include "Scene.inl"
#endif