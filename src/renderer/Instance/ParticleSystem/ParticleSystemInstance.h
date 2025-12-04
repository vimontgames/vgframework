#pragma once

#include "renderer/IParticleSystemInstance.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"
#include "core/Container/AtomicVector.h"
#include "renderer/Geometry/Vertex/VertexFormat.h"
#include "core/Misc/AABB/AABB.h"

namespace vg::renderer
{
    // For each view, store the offsets used to access GPU instance data and geometry data
    struct PerViewParticleData
    {
        const View * m_view = nullptr;
        core::uint   m_instanceDataOffset = -1;
        core::uint   m_geometryDataOffset = -1;
    };

    // TODO: SOA?
    struct Particle
    {
        float3  position;
        float3  velocity;
        float   lifetime;
        float   age;
        float3  size;
        float4  color;
        bool    alive;
    };

    class ParticleSystemInstance final : public IParticleSystemInstance
    {
    public:
        VG_CLASS_DECL(ParticleSystemInstance, IParticleSystemInstance);

                                    ParticleSystemInstance      (const core::string & _name, core::IObject * _parent);
                                    ~ParticleSystemInstance     ();

        GraphicInstanceType         GetGraphicIntanceType       () const final override { return GraphicInstanceType::ParticleSystem; }

        bool                        TryGetAABB                  (core::AABB & _aabb) const final override;
        bool                        Cull                        (const ViewCullingOptions & _cullingOptions, CullingResult * _cullingResult) final override;
        void                        Draw                        (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;

        bool                        SetEmitterCount             (core::uint _count) final override;
        bool                        SetEmitterParams            (core::uint _index, const ParticleEmitterParams & _emitterParams) final override;

        void                        UpdateTime                  (float _dt) final override;

        core::uint                  GetGPUInstanceDataSize      () const final override;
        core::uint                  FillGPUInstanceData         (const core::u8 * VG_RESTRICT _data) const final override;

        core::uint                  GetGPURenderDataSize        () const override;
        core::uint                  FillGPURenderData           (const core::u8 * VG_RESTRICT _data) override;

        void                        updateSimulation            ();

    private:
        float m_dt = 0.0f;

        struct EmitterData
        {
            ParticleEmitterParams   m_params;
            float                   m_timeSinceLastSpawn = 0.0f;
            core::vector<Particle>  m_particles;
            core::u16               m_aliveParticles = 0;
        };
        core::vector<EmitterData> m_emitters;
        core::AABB m_aabb;
    };
}

#if VG_ENABLE_INLINE
#include "ParticleSystemInstance.inl"
#endif