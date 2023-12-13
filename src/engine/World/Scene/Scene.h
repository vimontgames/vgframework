#pragma once

#include "core/IScene.h"

namespace vg::core
{
    class IGameObject;
    class GameObject;
}

namespace vg::engine
{

    class Scene : public core::IScene
    {
    public:
        VG_CLASS_DECL(Scene, core::IScene);

                                        Scene               (const core::string & _name, core::IObject * _parent);
                                        ~Scene              ();

        void                            SetRoot             (core::IGameObject * _sector) final;
        core::IGameObject *             GetRoot             () const final;

    public:
        void                            setRoot             (core::GameObject * _sector);
        VG_INLINE core::GameObject *    getRoot             () const;

    private:
        core::GameObject *              m_root              = nullptr;
    };
}

#if VG_ENABLE_INLINE
#include "Scene.inl"
#endif