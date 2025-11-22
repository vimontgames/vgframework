#include "engine/Precomp.h"
#include "ParticleComponent.h"
#include "core/GameObject/GameObject.h"
#include "renderer/IRenderer.h"
#include "renderer/IParticleSystemInstance.h"
#include "renderer/IPicking.h"
#include "engine/Engine.h"
#include "engine/Resource/Material/MaterialResourceData.h"
#include "editor/Editor_Consts.h"

#include "ParticleEmitterDesc.hpp"
#include "ParticleEmitterList.hpp"

#if !VG_ENABLE_INLINE
#include "ParticleComponent.inl"
#endif

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(ParticleComponent, "Particle", "Renderer", "Particle system for 3D rendering", editor::style::icon::Fire, 1)

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(ParticleComponent, m_maxParticleCount, "Max particles");
        setPropertyDescription(ParticleComponent, m_maxParticleCount, "Maximum particle count");
        setPropertyRange(ParticleComponent, m_maxParticleCount, uint2(1, 4096));

        registerPropertyObjectPtrEx(ParticleComponent, m_particleSystemInstance, "ParticleSystemInstance", PropertyFlags::Transient | PropertyFlags::Flatten);

        registerPropertyObject(ParticleComponent, m_particleEmitters, "Emitters");

        return true;
    }

    //--------------------------------------------------------------------------------------
    ParticleComponent::ParticleComponent(const core::string & _name, IObject * _parent) :
        core::Component(_name, _parent),
        m_particleEmitters(_name, this)
    {
        m_particleSystemInstance = (IParticleSystemInstance *)CreateFactoryObject(ParticleSystemInstance, _name, this);

        if (false == m_registered)
        {
            auto * picking = Engine::get()->GetRenderer()->GetPicking();
            PickingID id = m_particleSystemInstance->GetPickingID();
            if (!id)
            {
                id = picking->CreatePickingID(this);
                m_particleSystemInstance->SetPickingID(id);
            }

            getGameObject()->addGraphicInstance(m_particleSystemInstance);
            m_particleSystemInstance->SetName(getGameObject()->GetName().c_str());
            m_registered = true;
        }
    }

    //--------------------------------------------------------------------------------------
    ParticleComponent::~ParticleComponent()
    {
        // MeshComponent may have no PickingID if the mesh was not found
        auto * picking = Engine::get()->GetRenderer()->GetPicking();
        PickingID id = m_particleSystemInstance->GetPickingID();
        if (id)
            picking->ReleasePickingID(id);

        getGameObject()->removeGraphicInstance(m_particleSystemInstance);
        m_registered = false;
        Engine::get()->GetRenderer()->ReleaseAsync(m_particleSystemInstance);
    }

    //--------------------------------------------------------------------------------------
    void ParticleComponent::OnLoad()
    {
        const auto & emitters = m_particleEmitters.getObjects();
        m_particleSystemInstance->SetEmitterCount((uint)emitters.size());
        for (uint i = 0; i < emitters.size(); ++i)
        {
            m_particleSystemInstance->SetEmitterParams(i, emitters[i].getEmitterParams());

            MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(emitters[i].getMaterialResource().GetObject());
            m_particleSystemInstance->SetMaterial(i, matResData ? matResData->m_materialModel : nullptr);
        }
    }

    //--------------------------------------------------------------------------------------
    void ParticleComponent::Update(const Context & _context)
    {
        if (nullptr != m_particleSystemInstance)
        {
            m_particleSystemInstance->setGlobalMatrix(_context.m_gameObject->getGlobalMatrix());
            m_particleSystemInstance->setColor(_context.m_gameObject->getColor());
            m_particleSystemInstance->setObjectFlags(ObjectFlags::NoCulling, asBool(ComponentFlags::NoCulling & getComponentFlags()));
            m_particleSystemInstance->UpdateTime(_context.m_dt);
        }
    }

    //--------------------------------------------------------------------------------------
    void ParticleComponent::EnableComponentFlags(ComponentFlags _flags, bool _enabled)
    {
        super::EnableComponentFlags(_flags, _enabled);

        if (m_particleSystemInstance)
            m_particleSystemInstance->SetInstanceFlags(InstanceFlags::Enabled, asBool(ComponentFlags::Enabled & GetComponentFlags()));
    }

    //--------------------------------------------------------------------------------------
    const core::string ParticleComponent::GetSubObjectName(core::uint _subObjectIndex) const
    {
        //if (nullptr != m_particleSystemInstance)
        //    return m_particleSystemInstance->GetBatchName(_subObjectIndex);

        return "";
    };

    //--------------------------------------------------------------------------------------
    void ParticleComponent::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.GetName(), "m_particleEmitters"))
        {
            const auto & emitters = m_particleEmitters.getObjects();
            m_particleSystemInstance->SetEmitterCount((uint)emitters.size());
            for (uint i = 0; i < emitters.size(); ++i)
            {
                m_particleSystemInstance->SetEmitterParams(i, emitters[i].getEmitterParams());

                MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(emitters[i].getMaterialResource().GetObject());
                m_particleSystemInstance->SetMaterial(i, matResData ? matResData->m_materialModel : nullptr);
            }
        }
        else if (ParticleEmitterDesc * emitterDesc = dynamic_cast<ParticleEmitterDesc *>(_object))
        {
            const auto & emitters = m_particleEmitters.getObjects();
            for (uint i = 0; i < emitters.size(); ++i)
            {
                const auto & emitter = emitters[i];
                if (&emitter == emitterDesc)
                {
                    VG_INFO("[ParticleComponent] Emitter %u params changed", i);
                    m_particleSystemInstance->SetEmitterParams(i, emitterDesc->getEmitterParams());

                    MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(emitters[i].getMaterialResource().GetObject());
                    m_particleSystemInstance->SetMaterial(i, matResData ? matResData->m_materialModel : nullptr);

                    break;
                }
            }
        }
        else if (!strcmp(_prop.GetName(), "m_shader"))
        {
            MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(_object->GetParent());
            VG_ASSERT(matResData);
            MaterialResource * matRes = dynamic_cast<MaterialResource *>(matResData->GetParent());
            VG_ASSERT(matRes);
            const auto & emitters = m_particleEmitters.getObjects();
            for (uint i = 0; i < emitters.size(); ++i)
            {
                const auto & emitter = emitters[i];
                if (&emitter.getMaterialResource() == matRes)
                {
                    VG_INFO("[MeshComponent] Material %u shader changed", i);
                    MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(matRes->GetObject());
                    m_particleSystemInstance->SetMaterial(i, matResData ? matResData->m_materialModel : nullptr);
                    break;
                }
            }
        }
        else if (!strcmp(_prop.GetName(), "m_surfaceType") || !strcmp(_prop.GetName(), "m_cullMode"))
        {
            MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(_object->GetParent());
            VG_ASSERT(matResData);
            MaterialResource * matRes = dynamic_cast<MaterialResource *>(matResData->GetParent());
            VG_ASSERT(matRes);
            const auto & emitters = m_particleEmitters.getObjects();
            for (uint i = 0; i < emitters.size(); ++i)
            {
                const auto & emitter = emitters[i];
                if (&emitter.getMaterialResource() == matRes)
                {
                    m_particleSystemInstance->OnMaterialChanged();
                    break;
                }
            }
        }

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    void ParticleComponent::OnResourceLoaded(IResource * _resource)
    {
        if (auto matRes = dynamic_cast<MaterialResource *>(_resource))
        {
            // Material resource loaded
            const auto & emitters = m_particleEmitters.getObjects();
            for (uint i = 0; i < emitters.size(); ++i)
            {
                const auto & emitter = emitters[i];
                if (&emitter.getMaterialResource() == matRes)
                {
                    //VG_INFO("[ParticleComponent] Material %u loaded", i);
                    MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(matRes->GetObject());
                    m_particleSystemInstance->SetMaterial(i, matResData ? matResData->m_materialModel : nullptr);
                    break;
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ParticleComponent::OnResourceUnloaded(core::IResource * _resource)
    {
        if (auto matRes = dynamic_cast<MaterialResource *>(_resource))
        {
            // Material resource loaded
            const auto & emitters = m_particleEmitters.getObjects();
            for (uint i = 0; i < emitters.size(); ++i)
            {
                const auto & emitter = emitters[i];
                if (&emitter.getMaterialResource() == matRes)
                {
                    //VG_INFO("[ParticleComponent] Material %u unloaded", i);
                    MaterialResourceData * matResData = dynamic_cast<MaterialResourceData *>(matRes->GetObject());
                    m_particleSystemInstance->SetMaterial(i, nullptr);
                    break;
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::TryGetAABB(core::AABB & _aabb) const
    {
        if (m_particleSystemInstance)
        {
            if (m_particleSystemInstance->TryGetAABB(_aabb))
                return true;
        }

        return false;
    }
}
