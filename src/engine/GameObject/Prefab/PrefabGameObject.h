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

        void                                                SetName                     (const core::string & _name) final override;

        bool                                                IsPrefab                    () const override;
        core::IResource *                                   GetPrefabResource           () const override;

        IObject *                                           Instanciate                 () const;

        void                                                OnResourceLoaded            (core::IResource * _resource) override;
        void                                                OnResourceUnloaded          (core::IResource * _resource) override;

        void                                                OnLoad                      () override;

        void                                                OnPlay                      () override;
        void                                                OnStop                      () override;

        core::IDynamicPropertyList *                        GetDynamicPropertyList      (const core::IObject * _object) const override;
        core::IDynamicPropertyList *                        CreateDynamicPropertyList   (const core::IObject * _object) override;

        core::IDynamicProperty *                            GetDynamicProperty          (const core::IObject * _object, const core::IProperty * _prop) const override;
        bool                                                CanOverrideProperty         (const core::IObject * _object, const core::IProperty * _prop) const override;
        core::IDynamicProperty *                            CreateDynamicProperty       (const core::IObject * _object, const core::IProperty * _prop) override;
        bool                                                DestroyDynamicOverride      (const core::IObject * _object, const core::IProperty * _prop) override;
        void                                                OverrideGameObjectProperties(core::IGameObject * _gameObject, const core::IDynamicProperty * _dynProp) override;
        bool                                                EnablePropertyOverride      (const core::IObject * _object, const core::IProperty * _prop, bool _override) override;

    private:
        core::DynamicPropertyList *                         getDynamicPropertyList      (const core::IObject * _object) const;
        core::DynamicPropertyList *                         createDynamicPropertyList   (const core::IObject * _object);

        core::DynamicProperty *                             getDynamicProperty          (const core::IObject * _object, const core::IProperty * _prop) const;
        bool                                                canOverrideProperty         (const core::IObject * _object, const core::IProperty * _prop) const;
        core::DynamicProperty *                             createDynamicProperty       (const core::IObject * _object, const core::IProperty * _prop);
       bool                                                 destroyDynamicProperty      (const core::IObject * _object, const core::IProperty * _prop);

        void                                                patchPrefabGameObjectUIDs   (const core::IObject * _root, core::IObject * _object);

    private:
        PrefabResource                                      m_prefabResource;
        core::vector<core::DynamicPropertyList *>           m_dynamicProperties;
    };
}