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
        //m_perViewParticleData(16)
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
        _aabb = AABB(float3(-1,-1,-1), float3(1,1,1));  // TODO
        return true;
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
    bool ParticleSystemInstance::Cull(CullingResult * _cullingResult, View * _view) const
    {
        core::AABB aabb;
        if (TryGetAABB(aabb))
        {
            const bool visible = _view->getCameraFrustum().intersects(aabb, getGlobalMatrix()) != FrustumTest::Outside || asBool(ObjectFlags::NoCulling & getObjectFlags());

            if (visible)
            {
                // To update instance data
                _cullingResult->m_sharedOutput->addInstance(this);
                
                // Shared particle list (used for simulation)
                _cullingResult->m_sharedOutput->addParticleSystem(this);

                // All instances list (used e.g. to draw AABB)
                _cullingResult->m_output->add(GraphicInstanceListType::All, this);

                bool hasOpaque = false;
                bool hasAlphaTest = false;
                bool hasAlphaBlend = false;
                bool hasDecal = false;

                auto materials = getMaterials();
                if (materials.size() > 0)
                {
                    for (auto * material : materials)
                    {
                        if (material)
                        {
                            auto surf = material->getSurfaceType();

                            switch (surf)
                            {
                                case SurfaceType::Opaque:
                                    hasOpaque = true;
                                    break;

                                case SurfaceType::AlphaTest:
                                    hasAlphaTest = true;
                                    break;

                                case SurfaceType::AlphaBlend:
                                    hasAlphaBlend = true;
                                    break;
                            }
                        }
                        else
                        {
                            hasOpaque = true;
                            break;
                        }
                    }
                }
                else
                {
                    hasOpaque = true;
                }

                if (hasOpaque)
                    _cullingResult->m_output->add(GraphicInstanceListType::Opaque, this);

                if (hasAlphaTest)
                    _cullingResult->m_output->add(GraphicInstanceListType::AlphaTest, this);

                if (hasAlphaBlend)
                    _cullingResult->m_output->add(GraphicInstanceListType::Transparent, this);

                // Particle systems in this view (used to fill GPU data)
                _cullingResult->m_output->add(GraphicInstanceListType::Particle, this);

                // Selection outline
                const ObjectFlags objectFlags = this->getObjectFlags();
                if (asBool(ObjectFlags::Selected & objectFlags))
                    _cullingResult->m_output->add(GraphicInstanceListType::Outline, this);

                // In case of particles, each view needs its own instance data
                //if (!_view->isAdditionalView()) // TODO: register for "original" view in case of shadow maps (shadow map will use the same geometry as the view that is using it)
                //{
                //    PerViewParticleData perViewParticleData;
                //    perViewParticleData.m_view = _view;
                //    const auto index = m_perViewParticleData.push_back_atomic(perViewParticleData);
                //    m_perViewParticleData[index].m_instanceDataOffset = (uint)index;
                //}

                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleSystemInstance::OnUpdateRayTracing(gfx::CommandList * _cmdList, View * _view, core::uint _index)
    {
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
        //m_perViewParticleData.clear(); 

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

                aliveParticles++;
            }

            emitter.m_aliveParticles = aliveParticles;
        }
    }

    //--------------------------------------------------------------------------------------
    uint ParticleSystemInstance::GetGPUInstanceDataSize() const
    {
        //const uint viewCount = (uint)m_perViewParticleData.size();
        //const uint batchCount = (uint)m_emitters.size();
        //const uint gpuInstanceDataSize = alignUp((uint)(sizeof(GPUInstanceData) + batchCount * sizeof(GPUBatchData)), (uint)GPU_INSTANCE_DATA_ALIGNMENT);
        //return gpuInstanceDataSize * viewCount;

        const uint batchCount = (uint)m_emitters.size();
        const uint gpuInstanceDataSize = alignUp((uint)(sizeof(GPUInstanceData) + batchCount * sizeof(GPUBatchData)), (uint)GPU_INSTANCE_DATA_ALIGNMENT);
        return gpuInstanceDataSize;
    }

    //--------------------------------------------------------------------------------------
    uint ParticleSystemInstance::FillGPUInstanceData(const core::u8 * VG_RESTRICT _data) const
    {
        //const uint viewCount = (uint)m_perViewParticleData.size();
        const uint batchCount = (uint)m_emitters.size();

        const uint gpuInstanceDataSize = alignUp((uint)(sizeof(GPUInstanceData) + batchCount * sizeof(GPUBatchData)), (uint)GPU_INSTANCE_DATA_ALIGNMENT);

        VertexFormat vertexFormat = VertexFormat::Default;

        const auto * renderer = Renderer::get();
        const auto * defaultMaterial = renderer->getDefaultMaterial();
        VG_ASSERT(defaultMaterial);
        const auto defaultMaterialIndex = defaultMaterial->getGPUMaterialDataIndex();
        VG_ASSERT(defaultMaterialIndex == 0);

        uint dataOffset = 0;

        //for (uint i = 0; i < viewCount; ++i)
        {
            //const PerViewParticleData & perViewParticleData = m_perViewParticleData[i];
            
            //BindlessBufferHandle vbHandle = perViewParticleData.m_view->GetParticleVertexBuffer()->getBufferHandle();
            //const uint vbOffset = perViewParticleData.m_geometryDataOffset;

            BindlessBufferHandle vbHandle = getSkinnedMeshBuffer()->getBufferHandle();
            const uint vbOffset = getSkinnedMeshBufferOffset();

            GPUInstanceData * VG_RESTRICT instanceData = (GPUInstanceData * VG_RESTRICT)(_data + dataOffset);
            
            instanceData->setMaterialCount(batchCount);
            instanceData->setVertexFormat(vertexFormat);
            instanceData->setInstanceColor(getColor());
            //instanceData->setIndexBuffer(ibHandle, indexSize, ibOffset); // use not index buffer for particles or use default particle index buffer? ({0,1,2}, {1,2,3} ...)
            instanceData->setVertexBuffer(vbHandle, vbOffset);

            for (uint b = 0; b < batchCount; ++b)
            {
                GPUBatchData * VG_RESTRICT batchData = (GPUBatchData * VG_RESTRICT)(_data + dataOffset + sizeof(GPUInstanceData) + b * sizeof(GPUBatchData));
            
                const MaterialModel * mat = getMaterial(b);
                GPUMaterialDataIndex matIndex = (nullptr != mat) ? mat->getGPUMaterialDataIndex() : defaultMaterialIndex;
                batchData->setMaterialIndex(matIndex);
            
                //uint batchOffset = 0; // TODO support multiple particle batchs (nullptr != geo && b < geo->batches().size()) ? geo->batches()[b].offset : 0;
                //batchData->setStartIndex(batchOffset);
            }

            dataOffset += gpuInstanceDataSize;
        }

        return dataOffset;
    }

    //--------------------------------------------------------------------------------------
    core::uint  ParticleSystemInstance::GetGPURenderDataSize() const
    {
        uint totalSize = 0;
        for (uint i = 0; i < m_emitters.size(); ++i)
        {
            const auto & emitter = m_emitters[i];
            totalSize += (uint)emitter.m_aliveParticles * sizeof(ParticleVertex) * 6; // Alloc 6 vertices per particle
        }
        return totalSize;
    }

    //--------------------------------------------------------------------------------------
    core::uint ParticleSystemInstance::FillGPURenderData(const core::u8 * VG_RESTRICT _data)
    {
        //const PerViewParticleData * perViewParticleData = getPerViewParticleData(_view);
        //const uint offset = perViewParticleData->m_geometryDataOffset;
        const uint offset = getSkinnedMeshBufferOffset();
        ParticleVertex * VG_RESTRICT particleVertexData = (ParticleVertex * VG_RESTRICT)((u8 *)_data + offset);

        for (uint i = 0; i < m_emitters.size(); ++i)
        {
            const auto & emitter = m_emitters[i];
            const auto & particles = emitter.m_particles;

            for (uint p = 0; p < particles.size(); ++p)
            {
                const uint baseIndex = p * 6;
                const auto & particle = particles[p];

                if (!particle.alive)
                    continue;

                // For now just create a quad per particle
                ParticleVertex verts[6];

                float3 offset = particle.position;
                float3 scale = particle.size; // view->getViewID().id == 0 ? float3(0.5f, 0.5f, 0.5f) : float3(0.5f, 0.5f, 1.0f);
                u32 color = 0xFFFFFFFF; // view->getViewID().id == 0 ? 0xFFFF0000 : 0xFF0000FF;

                verts[0].setPos(offset + float3(-scale.x, 0.0f, +scale.y));
                verts[1].setPos(offset + float3(+scale.x, 0.0f, -scale.y));
                verts[2].setPos(offset + float3(-scale.x, 0.0f, -scale.y));

                verts[0].setColor(color);
                verts[1].setColor(color);
                verts[2].setColor(color);

                verts[0].setNormal(float3(0.0f, 1.0f, 0.0f));
                verts[1].setNormal(float3(0.0f, 1.0f, 0.0f));
                verts[2].setNormal(float3(0.0f, 1.0f, 0.0f));

                verts[0].setBinormal(float3(1.0f, 0.0f, 0.0f));
                verts[1].setBinormal(float3(1.0f, 0.0f, 0.0f));
                verts[2].setBinormal(float3(1.0f, 0.0f, 0.0f));

                verts[0].setTangent(float3(0.0f, 0.0f, 1.0f));
                verts[1].setTangent(float3(0.0f, 0.0f, 1.0f));
                verts[2].setTangent(float3(0.0f, 0.0f, 1.0f));

                verts[0].setUV0(float2(0, 0));
                verts[1].setUV0(float2(1, 1));
                verts[2].setUV0(float2(0, 1));

                verts[0].setUV1(float2(0, 0));
                verts[1].setUV1(float2(1, 1));
                verts[2].setUV1(float2(0, 1));

                verts[3].setPos(offset + float3(+scale.x, 0.0f, -scale.y));
                verts[4].setPos(offset + float3(-scale.x, 0.0f, +scale.y));
                verts[5].setPos(offset + float3(+scale.x, 0.0f, +scale.y));

                verts[3].setColor(color);
                verts[4].setColor(color);
                verts[5].setColor(color);

                verts[3].setNormal(float3(0.0f, 1.0f, 0.0f));
                verts[4].setNormal(float3(0.0f, 1.0f, 0.0f));
                verts[5].setNormal(float3(0.0f, 1.0f, 0.0f));

                verts[3].setBinormal(float3(1.0f, 0.0f, 0.0f));
                verts[4].setBinormal(float3(1.0f, 0.0f, 0.0f));
                verts[5].setBinormal(float3(1.0f, 0.0f, 0.0f));

                verts[3].setTangent(float3(0.0f, 0.0f, 1.0f));
                verts[4].setTangent(float3(0.0f, 0.0f, 1.0f));
                verts[5].setTangent(float3(0.0f, 0.0f, 1.0f));

                verts[3].setUV0(float2(1, 1));
                verts[4].setUV0(float2(0, 0));
                verts[5].setUV0(float2(1, 0));

                verts[3].setUV1(float2(1, 1));
                verts[4].setUV1(float2(0, 0));
                verts[5].setUV1(float2(1, 0));

                memcpy(particleVertexData, verts, sizeof(verts));
                particleVertexData += 6;
            }
        }

        return (uint)(uint_ptr(_data + offset) - (uint_ptr)particleVertexData);
    }

    //--------------------------------------------------------------------------------------
    void ParticleSystemInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        auto * renderer = Renderer::get();
        const float4x4 world = getGlobalMatrix();

        //const View * view = (View *)_renderContext.m_renderPass->getView();
        //const PerViewParticleData * perViewParticleData = getPerViewParticleData(view);

       // VG_ASSERT(nullptr != perViewParticleData);
        //if (nullptr == perViewParticleData)
        //    return;

        RootConstants3D root3D;
        root3D.setWorldMatrix(transpose(world));

        //const uint gpuInstanceDataSize = alignUp((uint)(sizeof(GPUInstanceData) + 1 * sizeof(GPUBatchData)), (uint)GPU_INSTANCE_DATA_ALIGNMENT);
        //root3D.setGPUInstanceDataOffset(getGPUInstanceDataOffset() + perViewParticleData->m_instanceDataOffset * gpuInstanceDataSize);
        root3D.setGPUInstanceDataOffset(getGPUInstanceDataOffset());

        //BindlessBufferHandle vbHandle = view->GetParticleVertexBuffer()->getBufferHandle();
        //uint vbOffset = perViewParticleData->m_geometryDataOffset;
        BindlessBufferHandle vbHandle = getSkinnedMeshBuffer()->getBufferHandle();
        uint vbOffset = getSkinnedMeshBufferOffset();
        //VG_VERIFY(GetVertexBuffer(vbHandle, vbOffset));

        root3D.setVertexBufferHandle(vbHandle, vbOffset);

        u16 flags = 0;
        root3D.setFlags(flags);

        VertexFormat vertexFormat = VertexFormat::Default;
        //VG_VERIFY(GetVertexFormat(vertexFormat));
        root3D.setVertexFormat(vertexFormat);

        auto pickingID = GetPickingID();
        root3D.setPickingID(pickingID);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        const BitMask & batchMask = getBatchMask();

        const auto & emitters = m_emitters;
        for (uint i = 0; i < emitters.size(); ++i)
        {
            const auto & emitter = emitters[i];
            if (!emitter.m_aliveParticles)
                continue;

            bool draw = false;

            const MaterialModel * material = getMaterial(i);
            if (nullptr == material)
                material = renderer->getDefaultMaterial();

            auto surfaceType = material->getSurfaceType();

            if (_renderContext.m_surfaceType != surfaceType && !_renderContext.m_wireframe && !_renderContext.m_outline)
                continue;

            switch (_renderContext.m_shaderPass)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_renderContext.m_shaderPass);
                    continue;
                    break;

                case ShaderPass::ZOnly:
                case ShaderPass::Forward:
                case ShaderPass::Deferred:
                case ShaderPass::Outline:
                    material->Setup(_renderContext, _cmdList, &root3D, i);
                    break;

                case ShaderPass::Transparent:
                    material->Setup(_renderContext, _cmdList, &root3D, i);
                    break;
            }

            const uint vertexCount = (uint)emitter.m_aliveParticles;
            const uint vertexOffset = vbOffset;

            _cmdList->setGraphicRootConstants(0, (u32 *)&root3D, RootConstants3DCount);
            _cmdList->draw(vertexCount*6, 0);

            vbOffset += vertexCount * sizeof(ParticleVertex);
        }
    }
}
