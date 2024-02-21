#pragma once

#include "core/GameObject/GameObject.h"
#include "engine/Resource/BaseScene/Prefab/PrefabResource.h"

namespace vg::engine
{
    class PrefabGameObject : public core::GameObject
    {
    public:
        VG_CLASS_DECL(PrefabGameObject, GameObject);

                                                            PrefabGameObject        (const core::string & _name, core::IObject * _parent);
        virtual                                             ~PrefabGameObject       ();

        bool                                                IsPrefab                () const override;
        core::IResource *                                   GetPrefabResource       () const override;

        void                                                onResourceLoaded        (core::IResource * _resource) override;
        void                                                onResourceUnloaded      (core::IResource * _resource) override;

        //void                                                OnPlay                  () override;
        //void                                                OnStop                  () override;

        //const core::vector<core::IGameObject *> &           GetChildren             () const override;

    private:
        PrefabResource                                      m_prefabResource;
    };
}