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
        using super = IScene;

                                Scene               (const string & _name, IObject * _parent);
                                ~Scene              ();

        const char *            getClassName        () const final { return "Scene"; }

        static bool             registerClass       (IFactory & _factory);
        static bool             registerProperties  (IClassDesc & _desc);

        static bool             load                (IObject * _object);
        static bool             save                (IObject * _object);

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