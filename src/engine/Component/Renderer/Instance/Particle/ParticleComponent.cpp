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
    bool ParticleComponent::playAllParticleEmitters(IObject * _object)
    {
        return ((ParticleComponent *)_object)->Play() && ((ParticleComponent *)_object)->Reset();
    }

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::pauseAllParticleEmitters(IObject * _object)
    {
        return ((ParticleComponent *)_object)->Pause();
    }

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::stopAllParticleEmitters(IObject * _object)
    {
        return ((ParticleComponent *)_object)->Stop();
    }

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyCallbackEx(ParticleComponent, playAllParticleEmitters, editor::style::icon::Play, PropertyFlags::SingleLine);
        setPropertyDescription(ParticleComponent, playAllParticleEmitters, "Play all emitters");

        registerPropertyCallbackEx(ParticleComponent, pauseAllParticleEmitters, editor::style::icon::Pause, PropertyFlags::SingleLine);
        setPropertyDescription(ParticleComponent, pauseAllParticleEmitters, "Pause all emitters");

        registerPropertyCallbackEx(ParticleComponent, stopAllParticleEmitters, editor::style::icon::Stop, PropertyFlags::SingleLine);
        setPropertyDescription(ParticleComponent, stopAllParticleEmitters, "Stop all emitters");

        registerProperty(ParticleComponent, m_playOnStart, "Play On Start");
        setPropertyDescription(ParticleComponent, m_playOnStart, "Play particle system when game starts");

        registerProperty(ParticleComponent, m_worldSpace, "World Space");
        setPropertyDescription(ParticleComponent, m_worldSpace, "Simulate particle in world space");

        registerPropertyObject(ParticleComponent, m_particleEmitters, editor::style::label::particle::Emitters);

        registerPropertyObjectPtrEx(ParticleComponent, m_particleSystemInstance, "ParticleSystemInstance", PropertyFlags::Transient | PropertyFlags::Flatten);

        return true;
    }

    //--------------------------------------------------------------------------------------
    ParticleComponent::ParticleComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent),
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

            registerGraphicInstance();
            m_particleSystemInstance->SetName(getGameObject()->GetName().c_str());
            m_particleSystemInstance->SetWorldSpace(m_worldSpace);

            // Initial refresh
            UpdateFlagsFromGameObject();
            RefreshGraphicInstance();
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
    bool ParticleComponent::Play()
    {
        auto & emitters = m_particleEmitters.getObjects();
        for (uint i = 0; i < emitters.size(); ++i)
        {
            auto & params = emitters[i].getEmitterParams();
            params.m_play = true;
            params.m_spawn = true;
            m_particleSystemInstance->SetEmitterParams(i, params);
        }
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::Pause()
    {
        auto & emitters = m_particleEmitters.getObjects();
        for (uint i = 0; i < emitters.size(); ++i)
        {
            auto & params = emitters[i].getEmitterParams();
            params.m_play = !params.m_play;
            m_particleSystemInstance->SetEmitterParams(i, params);
        }
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::Stop()
    {
        auto & emitters = m_particleEmitters.getObjects();
        for (uint i = 0; i < emitters.size(); ++i)
        {
            auto & params = emitters[i].getEmitterParams();
            params.m_spawn = false;
            m_particleSystemInstance->SetEmitterParams(i, params);
        }
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::Reset()
    {
        auto & emitters = m_particleEmitters.getObjects();
        for (uint i = 0; i < emitters.size(); ++i)
        {
            auto & params = emitters[i].getEmitterParams();
            m_particleSystemInstance->ResetEmitter(i);
        }
        return true;
    }


    //--------------------------------------------------------------------------------------
    void ParticleComponent::SetSpawnRate(core::uint _index, float _spawnRate)
    {
        auto & emitters = m_particleEmitters.getObjects();
        VG_ASSERT(_index < emitters.size(), "[Particle] Emitter %u not found in \"%s\"", GetGameObject()->GetName().c_str());

        if (_index < emitters.size())
        {
            auto & params = emitters[_index].getEmitterParams();
            params.m_spawnRate = _spawnRate;
            m_particleSystemInstance->SetEmitterParams(_index, params);
        }
    }

    //--------------------------------------------------------------------------------------
    float ParticleComponent::GetSpawnRate(core::uint _index) const
    {
        auto & emitters = m_particleEmitters.getObjects();
        VG_ASSERT(_index < emitters.size(), "[Particle] Emitter %u not found in \"%s\"", GetGameObject()->GetName().c_str());

        if (_index < emitters.size())
            return emitters[_index].getEmitterParams().m_spawnRate;
        
        return 0.0f;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::UpdateEmitter(core::uint _index)
    {
        const auto & emitters = m_particleEmitters.getObjects();
        if (_index < emitters.size())
        {
            m_particleSystemInstance->SetEmitterParams(_index, emitters[_index].getEmitterParams());
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::ResetEmitter(core::uint _index)
    {
        const auto & emitters = m_particleEmitters.getObjects();
        if (_index < emitters.size())
        {
            m_particleSystemInstance->ResetEmitter(_index);
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    void ParticleComponent::RefreshGraphicInstance()
    {
        if (nullptr != m_particleSystemInstance)
        {
            const IGameObject * go = GetGameObject();

            m_particleSystemInstance->setGlobalMatrix(go->getGlobalMatrix());
            m_particleSystemInstance->setColor(go->getColor());

            // This should be done here, and not every frame
            m_particleSystemInstance->setObjectFlags(ObjectFlags::NoCulling, asBool(ComponentFlags::NoCulling & getComponentFlags()));
        }
    }

    //--------------------------------------------------------------------------------------
    void ParticleComponent::Update(const Context & _context)
    {
        // 'Static' instance do not need refresh every frame, but a 'Static' particle system does not make much sense
        RefreshGraphicInstance();

        // Update time
        if (nullptr != m_particleSystemInstance)
        {
            auto * engine = Engine::get();
            const float dt = engine->IsPlaying() ? _context.m_dt : engine->getTime().m_realDeltaTime;
            m_particleSystemInstance->UpdateTime(dt);
        }
    }

    //--------------------------------------------------------------------------------------
    void ParticleComponent::OnPlay()
    {
        Reset();
        if (m_playOnStart)
            Play();
    }

    //--------------------------------------------------------------------------------------
    void ParticleComponent::OnStop()
    {
        Stop();
        Reset();
    }

    //--------------------------------------------------------------------------------------
    void ParticleComponent::EnableComponentFlags(ComponentFlags _flags, bool _enabled)
    {
        super::EnableComponentFlags(_flags, _enabled);

        if (m_particleSystemInstance)
            m_particleSystemInstance->SetInstanceFlags(InstanceFlags::Enabled, asBool(ComponentFlags::Enabled & GetComponentFlags()));
    }

    //--------------------------------------------------------------------------------------
    core::uint ParticleComponent::getEmitterIndex(ParticleEmitterDesc * _emitter) const
    {
        const auto & emitters = m_particleEmitters.getObjects();
        for (uint i = 0; i < emitters.size(); ++i)
        {
            if (_emitter == &emitters[i])
                return i;
        }
        return (uint)-1;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::onEmittersSwap(core::uint _indexA, core::uint _indexB)
    {
        VG_DEBUGPRINT("[Particle] Swap render data for emitters %u and %u", _indexA, _indexB);

        if (nullptr != m_particleSystemInstance)
            return m_particleSystemInstance->OnEmittersSwap(_indexA, _indexB);

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleComponent::onEmitterRemoved(core::uint _index)
    {
        VG_DEBUGPRINT("[Particle] Remove emitters at index %u", _index);

        if (nullptr != m_particleSystemInstance)
            return m_particleSystemInstance->OnEmittersRemoved(_index);

        return false;
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
        if (!strcmp(_prop.GetName(), "m_worldSpace"))
        {
            m_particleSystemInstance->SetWorldSpace(m_worldSpace);
        }
        else if (!strcmp(_prop.GetName(), "m_particleEmitters"))
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
