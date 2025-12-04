#include "ParticleSystemInstance.h"
#include "renderer/Particle/ParticleManager.h"

#if !VG_ENABLE_INLINE
#include "ParticleSystemInstance.inl"
#endif

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS_EX(ParticleSystemInstance, "ParticleSystem Instance", core::ClassDescFlags::Instance);

    //--------------------------------------------------------------------------------------
    bool ParticleSystemInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    ParticleSystemInstance::ParticleSystemInstance(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        ParticleManager::get()->registerParticleSystemInstance(this);
    }

    //--------------------------------------------------------------------------------------
    ParticleSystemInstance::~ParticleSystemInstance()
    {
        ParticleManager::get()->unregisterParticleSystemInstance(this);
    }

    //--------------------------------------------------------------------------------------
    bool ParticleSystemInstance::TryGetAABB(core::AABB & _aabb) const
    {
        if (m_aabb.isFinite())
        {
            _aabb = m_aabb;
            return true;
        }
        else
        {
            return false;
        }
    }

    //--------------------------------------------------------------------------------------
    bool ParticleSystemInstance::SetEmitterCount(core::uint _count)
    {
        if (m_emitters.size() != _count)
        {
            m_emitters.resize(_count);
            SetMaterialCount(_count);
            return true;
        }
        return false;
    }
      
    //--------------------------------------------------------------------------------------
    bool ParticleSystemInstance::SetEmitterParams(core::uint _index, const ParticleEmitterParams & _emitterParams)
    {
        if (m_emitters.size() < _index + 1)
            m_emitters.resize(_index + 1);

        auto & emitter = m_emitters[_index];

        if (emitter.m_params != _emitterParams)
        {
            emitter.m_params = _emitterParams;
            emitter.m_particles.resize(_emitterParams.m_maxParticleCount);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleSystemInstance::Cull(const ViewCullingOptions & _cullingOptions, CullingResult * _cullingResult)
    {
        core::AABB aabb;
        if (TryGetAABB(aabb))
        {
            const bool visible = _cullingOptions.m_view->getCameraFrustum().intersects(aabb, getGlobalMatrix()) != FrustumTest::Outside || asBool(ObjectFlags::NoCulling & getObjectFlags());

            if (visible)
            {
                // To update instance data
                _cullingResult->m_sharedOutput->addInstance(this);
                
                // Shared particle list (used for simulation)
                _cullingResult->m_sharedOutput->addParticleSystem(this);

                // All instances list (used e.g. to draw AABB)
                _cullingResult->m_output->add(GraphicInstanceListType::All, this);

                const auto surfaceTypes = computeSurfaceTypeFlags();

                if (asBool(SurfaceTypeFlags::Opaque & surfaceTypes))
                    _cullingResult->m_output->add(GraphicInstanceListType::Opaque, this);

                if (asBool(SurfaceTypeFlags::AlphaTest & surfaceTypes))
                    _cullingResult->m_output->add(GraphicInstanceListType::AlphaTest, this);

                if (asBool(SurfaceTypeFlags::AlphaBlend & surfaceTypes))
                    _cullingResult->m_output->add(GraphicInstanceListType::Transparent, this);

                // Particle systems in this view (used to fill GPU data)
                _cullingResult->m_output->add(GraphicInstanceListType::Particle, this);

                // Selection outline
                if (asBool(ObjectRuntimeFlags::Selected & getObjectRuntimeFlags()))
                    _cullingResult->m_output->add(GraphicInstanceListType::Outline, this);

                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void ParticleSystemInstance::UpdateTime(float _dt)
    {
        m_dt = _dt;
        for (uint i = 0; i < m_emitters.size(); ++i)
            m_emitters[i].m_timeSinceLastSpawn += _dt;
    }

    //--------------------------------------------------------------------------------------
    void ParticleSystemInstance::updateSimulation()
    {
        removeAtomicFlags(GraphicInstance::AtomicFlags::ParticleList);

        AABB aabb;
        aabb.reset();

        for (uint i = 0; i < m_emitters.size(); ++i)
        {
            auto & emitter = m_emitters[i];

            // 1. Spawn
            float spawnRate = emitter.m_params.m_spawnRate;
            const uint numToSpawn = (int)floor(spawnRate * emitter.m_timeSinceLastSpawn);
            if (numToSpawn > 0)
            {
                emitter.m_timeSinceLastSpawn -= numToSpawn / spawnRate;
                for (uint i = 0; i < numToSpawn; ++i)
                {
                    // Find dead particle
                    uint index = -1;
                    for (uint p = 0; p < emitter.m_particles.size(); ++p)
                    {
                        Particle & particle = emitter.m_particles[p];
                        if (!particle.alive)
                        {
                            index = p;
                            break;
                        }
                    }
                    if (-1 == index)
                    {
                        if (emitter.m_particles.size() < emitter.m_params.m_maxParticleCount)
                        {
                            emitter.m_particles.emplace_back();
                            index = (uint)emitter.m_particles.size() - 1;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    // New particle
                    Particle & particle = emitter.m_particles[index];
                    particle.position = float3(0, 0, 0);
                    particle.velocity = float3(0, 1, 0);
                    particle.lifetime = emitter.m_params.m_lifeTime;
                    particle.age = 0.0f;
                    particle.size = emitter.m_params.m_size;
                    particle.color = float4(1, 1, 1, 1);
                    particle.alive = true;
                }
            }

            // 2. Update
            uint aliveParticles = 0;
            for (uint p = 0; p < emitter.m_particles.size(); ++p)
            {
                Particle & particle = emitter.m_particles[p];

                if (!particle.alive)
                    continue;

                particle.age += m_dt;

                if (particle.age >= particle.lifetime)
                {
                    particle.alive = false;
                    continue;
                }

                particle.velocity = float3(0, 0, 100) * m_dt;
                particle.position += particle.velocity * m_dt;

                const ParticleEmitterParams & params = emitter.m_params;
                const FloatCurve & opacityCurve = params.m_opacity;

                if (opacityCurve.getCurveCount() == 1)
                {
                    const float t = particle.age;

                    float4 color = float4(1, 1, 1, 1);
                    color.a = opacityCurve.getValue(t, 0);

                    particle.color = color;
                }
                else if (opacityCurve.getCurveCount() == 4)
                {
                    const float t = particle.age;

                    float4 color = float4(1, 1, 1, 1);
                    color.r = opacityCurve.getValue(t, 0);
                    color.g = opacityCurve.getValue(t, 1);
                    color.b = opacityCurve.getValue(t, 2);
                    color.a = opacityCurve.getValue(t, 3);

                    particle.color = color;
                }

                const float radius = max(particle.size.x, max(particle.size.y, particle.size.z)) * 0.5f;

                aabb.grow(particle.position - radius);
                aabb.grow(particle.position + radius);

                aliveParticles++;
            }

            emitter.m_aliveParticles = aliveParticles;
        }
        m_aabb = aabb;
    }

    //--------------------------------------------------------------------------------------
    uint ParticleSystemInstance::GetGPUInstanceDataSize() const
    {
        const uint batchCount = (uint)m_emitters.size();
        const uint gpuInstanceDataSize = alignUp((uint)(sizeof(GPUInstanceData) + batchCount * sizeof(GPUBatchData)), (uint)GPU_INSTANCE_DATA_ALIGNMENT);
        return gpuInstanceDataSize;
    }

    //--------------------------------------------------------------------------------------
    uint ParticleSystemInstance::FillGPUInstanceData(const core::u8 * VG_RESTRICT _data) const
    {
        const uint batchCount = (uint)m_emitters.size();

        const uint gpuInstanceDataSize = alignUp((uint)(sizeof(GPUInstanceData) + batchCount * sizeof(GPUBatchData)), (uint)GPU_INSTANCE_DATA_ALIGNMENT);

        VertexFormat vertexFormat = VertexFormat::Default;

        const auto * renderer = Renderer::get();
        const auto * defaultMaterial = renderer->getDefaultMaterial();
        VG_ASSERT(defaultMaterial);
        const auto defaultMaterialIndex = defaultMaterial->getGPUMaterialDataIndex();
        VG_ASSERT(defaultMaterialIndex == 0);

        uint dataOffset = 0;

        BindlessBufferHandle ibHandle = getInstanceIndexBuffer()->getBufferHandle();
        const uint ibOffset = getInstanceIndexBufferOffset();
        const uint indexSize = getInstanceIndexBuffer()->getBufDesc().elementSize;

        BindlessBufferHandle vbHandle = getInstanceVertexBuffer()->getBufferHandle();
        const uint vbOffset = getInstanceVertexBufferOffset();

        GPUInstanceData * VG_RESTRICT instanceData = (GPUInstanceData * VG_RESTRICT)(_data + dataOffset);
            
        instanceData->setMaterialCount(batchCount);
        instanceData->setVertexFormat(vertexFormat);
        instanceData->setInstanceColor(getColor());
        instanceData->setIndexBuffer(ibHandle, indexSize, ibOffset); // use not index buffer for particles or use default particle index buffer? ({0,1,2}, {1,2,3} ...)
        instanceData->setVertexBuffer(vbHandle, vbOffset);

        for (uint b = 0; b < batchCount; ++b)
        {
            GPUBatchData * VG_RESTRICT batchData = (GPUBatchData * VG_RESTRICT)(_data + dataOffset + sizeof(GPUInstanceData) + b * sizeof(GPUBatchData));
            
            const MaterialModel * mat = getMaterial(b);
            GPUMaterialDataIndex matIndex = (nullptr != mat) ? mat->getGPUMaterialDataIndex() : defaultMaterialIndex;
            batchData->setMaterialIndex(matIndex);
        }

        dataOffset += gpuInstanceDataSize;

        return dataOffset;
    }

    //--------------------------------------------------------------------------------------
    core::uint  ParticleSystemInstance::GetGPURenderDataSize() const
    {
        uint totalSize = 0;
        for (uint i = 0; i < m_emitters.size(); ++i)
        {
            const auto & emitter = m_emitters[i];
            totalSize += (uint)emitter.m_aliveParticles * sizeof(ParticleQuadVertex); // Alloc 1 "Vertex" per particle
        }
        return totalSize;
    }

    //--------------------------------------------------------------------------------------
    core::uint ParticleSystemInstance::FillGPURenderData(const core::u8 * VG_RESTRICT _data)
    {
        const uint offset = getInstanceVertexBufferOffset();
        ParticleQuadVertex * VG_RESTRICT particleQuadData = (ParticleQuadVertex * VG_RESTRICT)((u8 *)_data + offset);

        for (uint i = 0; i < m_emitters.size(); ++i)
        {
            const auto & emitter = m_emitters[i];
            const auto & particles = emitter.m_particles;

            // TODO: Format is 28 bytes, so in order to get multiples of 128 bytes for faster memcpy we should process by 32 (16*32=896=7*128) 
            // SOA using waves of 32 particles for simulation + transpose would be even better
            for (uint p = 0; p < particles.size(); ++p)
            {
                const auto & particle = particles[p];

                if (!particle.alive)
                    continue;

                // For now just create a quad per particle
                ParticleQuadVertex quad;

                float3 size = particle.size;
                u32 color = packRGBA8(particle.color); 
               
                quad.setPos(particle.position);
                quad.setFrame(0);
                quad.setSize(particle.size.xy * 0.5f);
                quad.setColor(color);               
                
                memcpy(particleQuadData, &quad, sizeof(ParticleQuadVertex));
                particleQuadData++;
            }
        }

        return (uint)(uint_ptr(_data + offset) - (uint_ptr)particleQuadData);
    }

    //--------------------------------------------------------------------------------------
    void ParticleSystemInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        auto * renderer = Renderer::get();

        RenderContext renderContext = _renderContext;
        renderContext.m_particle = true;

        const float4x4 world = getGlobalMatrix();

        RootConstants3D root3D;
        root3D.setWorldMatrix(transpose(world));
        root3D.setGPUInstanceDataOffset(getGPUInstanceDataOffset());

        Buffer * ib = getInstanceIndexBuffer();
        _cmdList->setIndexBuffer(ib);

        BindlessBufferHandle vbHandle = getInstanceVertexBuffer()->getBufferHandle();
        uint vbOffset = getInstanceVertexBufferOffset();

        u16 flags = 0;
        root3D.setFlags(flags);
        root3D.setVertexFormat(VertexFormat::ParticleQuad);

        auto pickingID = GetPickingID();
        root3D.setPickingID(pickingID);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        const BitMask & batchMask = getBatchMask();

        const auto & emitters = m_emitters;
        for (uint i = 0; i < emitters.size(); ++i)
        {
            const auto & emitter = emitters[i];
            const uint particleCount = (uint)emitter.m_aliveParticles;
            if (particleCount > 0)
            {
                const MaterialModel * material = getMaterial(i);
                if (nullptr == material)
                    material = renderer->getDefaultMaterial();

                auto surfaceType = material->getSurfaceType();

                if (renderContext.m_surfaceType == surfaceType || renderContext.m_wireframe || renderContext.m_outline)
                {
                    switch (renderContext.m_shaderPass)
                    {
                        default:
                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(renderContext.m_shaderPass);
                            continue;
                            break;

                        case ShaderPass::ZOnly:
                        case ShaderPass::Forward:
                        case ShaderPass::Deferred:
                        case ShaderPass::Outline:
                            material->Setup(renderContext, _cmdList, &root3D, i);
                            break;

                        case ShaderPass::Transparent:
                            material->Setup(renderContext, _cmdList, &root3D, i);
                            break;
                    }

                    const uint vertexOffset = vbOffset;

                    root3D.setVertexBufferHandle(vbHandle, vbOffset);
                    _cmdList->setGraphicRootConstants(0, (u32 *)&root3D, RootConstants3DCount);
                    _cmdList->drawIndexed(particleCount * 6);
                }
            }

            vbOffset += particleCount * sizeof(ParticleQuadVertex);
        }
    }
}
