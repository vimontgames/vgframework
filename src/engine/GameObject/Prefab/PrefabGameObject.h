#pragma once

#include "core/GameObject/GameObject.h"
#include "engine/Resource/BaseScene/Prefab/PrefabResource.h"

namespace vg::core
{
    class IDynamicProperty;
    class DynamicProperty;

    class IDynamicPropertyList;
    class DynamicPropertyList;
}

namespace vg::engine
{
    class PropertyOverrides;

    class PrefabGameObject : public core::GameObject
    {
    public:
        VG_CLASS_DECL(PrefabGameObject, GameObject);

                                                            PrefabGameObject            (const core::string & _name, core::IObject * _parent);
        virtual                                             ~PrefabGameObject           ();

        bool                                                IsPrefab                    () const override;
        core::IResource *                                   GetPrefabResource           () const override;

        void                                                onResourceLoaded            (core::IResource * _resource) override;
        void                                                onResourceUnloaded          (core::IResource * _resource) override;

        void                                                OnLoad                      () override;

        //void                                              OnPlay                      () override;
        //void                                              OnStop                      () override;

        //const core::vector<core::IGameObject *> &         GetChildren                 () const override;

        core::IDynamicPropertyList *                        GetDynamicPropertyList      (const core::IObject * _object) const override;
        core::IDynamicPropertyList *                        CreateDynamicPropertyList   (const core::IObject * _object) override;

        core::IDynamicProperty *                            GetDynamicProperty          (const core::IObject * _object, const core::IProperty * _prop) const override;
        bool                                                CanOverrideProperty         (const core::IObject * _object, const core::IProperty * _prop) const override;
        core::IDynamicProperty *                            CreateDynamicProperty       (const core::IObject * _object, const core::IProperty * _prop) override;
        void                                                OverrideGameObjectProperties(core::IGameObject * _gameObject, const core::IProperty * _prop = nullptr) override;

    private:
        core::DynamicPropertyList *                         getDynamicPropertyList      (const core::IObject * _object) const;
        core::DynamicPropertyList *                         createDynamicPropertyList   (const core::IObject * _object);

        core::DynamicProperty *                             getDynamicProperty          (const core::IObject * _object, const core::IProperty * _prop) const;
        bool                                                canOverrideProperty         (const core::IObject * _object, const core::IProperty * _prop) const;
        core::DynamicProperty *                             createDynamicProperty       (const core::IObject * _object, const core::IProperty * _prop);

       

    private:
        PrefabResource                                      m_prefabResource;
        core::vector<core::DynamicPropertyList *>           m_gameObjects;
    };
}