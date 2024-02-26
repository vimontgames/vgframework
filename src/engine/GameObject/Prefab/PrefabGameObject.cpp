#include "engine/Precomp.h"
#include "PrefabGameObject.h"
#include "engine/World/Prefab/PrefabScene.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS_EX(PrefabGameObject, "PrefabGameObject", IClassDesc::Flags::GameObject);

    //--------------------------------------------------------------------------------------
    bool PrefabGameObject::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyResource(PrefabGameObject, m_prefabResource, "File");
        
        return true;
    }

    //--------------------------------------------------------------------------------------
    PrefabGameObject::PrefabGameObject(const core::string & _name, IObject * _parent) :
        GameObject(_name, _parent)
    {
        m_prefabResource.setParent(this);
    }

    //--------------------------------------------------------------------------------------
    PrefabGameObject::~PrefabGameObject()
    {
      
    }

    //--------------------------------------------------------------------------------------
    bool PrefabGameObject::IsPrefab() const
    {
        return true;
    }

    //--------------------------------------------------------------------------------------
    // When not instanced, 
    // a prefab will return children from its prefab scene for rendering but will not update them.
    //--------------------------------------------------------------------------------------
    //const vector<IGameObject *> & PrefabGameObject::GetChildren() const
    //{
    //    if (asBool(UpdateFlags::InstancedPrefab & getUpdateFlags()))
    //    {
    //        return super::GetChildren();
    //    }
    //    else
    //    {
    //        if (PrefabScene * prefabScene = (PrefabScene*)m_prefabResource.getObject())
    //        {
    //            return prefabScene->GetRoot()->GetChildren();
    //        }
    //        else
    //        {
    //            const static vector<IGameObject *> empty;
    //            return empty;
    //        }
    //    }
    //}

    //--------------------------------------------------------------------------------------
    core::IResource * PrefabGameObject::GetPrefabResource() const
    {
        VG_ASSERT(IsPrefab());
        return (core::IResource *) &m_prefabResource;
    }

    //--------------------------------------------------------------------------------------
    void PrefabGameObject::onResourceLoaded(IResource * _resource)
    {
        if (_resource == &m_prefabResource)
        {
            // Instanciate the Prefab
            if (PrefabScene * prefabScene = VG_SAFE_STATIC_CAST(PrefabScene, _resource->getObject()))
            {
                IGameObject * prefabRoot = prefabScene->GetRoot();

                auto & prefabChildren = GetChildren();
                if (prefabChildren.size() == 0)
                {
                    IGameObject * instance = VG_SAFE_STATIC_CAST(IGameObject, prefabRoot->Instanciate());
                    instance->SetObjectFlags(ObjectFlags::NotSerialized, true);
                    instance->setParent(this);

                    AddChild(instance);
                    VG_SAFE_RELEASE(instance);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void PrefabGameObject::onResourceUnloaded(IResource * _resource)
    {
        if (_resource == &m_prefabResource)
        {
            // Destroy the Prefab instances
            auto & prefabChildren = GetChildren();

            if (prefabChildren.size() > 0)
            {
                auto * prefabChild = prefabChildren[0];
                prefabChild->setParent(nullptr);
                RemoveChild(prefabChild, false);
            }
        }
    }

    ////--------------------------------------------------------------------------------------
    //void PrefabGameObject::OnPlay()
    //{
    //    
    //}
    //
    ////--------------------------------------------------------------------------------------
    //void PrefabGameObject::OnStop()
    //{
    //   
    //}
}