#include "ParticleRenderDataUpdatePass.h"
#include "renderer/Instance/ParticleSystem/ParticleSystemInstance.h"

#define GPU_PARTICLE_MAX_COUNT_PER_EMITTER 4096     // Max particle count per emitter
#define GPU_PARTICLE_MAX_COUNT_PER_FRAME 16384      // Total max particle count rendered per frame

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ParticleRenderDataUpdatePass::ParticleRenderDataUpdatePass() :
        UpdatePass("ParticleDataUpdatePass")
    {
        auto * device = Device::get();

        u16 * ibInitData = new u16[GPU_PARTICLE_MAX_COUNT_PER_EMITTER * 6]; // 6 indices and 4 vertices per quad
        for (uint i = 0; i < GPU_PARTICLE_MAX_COUNT_PER_EMITTER; ++i)
        {
            const uint indexOffset = i * 6;
            const uint quadOffset = i * 4;

            // 0------1
            // | \    |
            // |  \   |
            // |   \  |
            // |    \ |
            // |     \|
            // 3------2

            ibInitData[indexOffset + 0] = quadOffset + 0;
            ibInitData[indexOffset + 1] = quadOffset + 1;
            ibInitData[indexOffset + 2] = quadOffset + 2;

            ibInitData[indexOffset + 3] = quadOffset + 2;
            ibInitData[indexOffset + 4] = quadOffset + 1;
            ibInitData[indexOffset + 5] = quadOffset + 3;
        }

        BufferDesc particleStaticIndexBufferDesc = BufferDesc(Usage::Default, BindFlags::IndexBuffer, CPUAccessFlags::None, BufferFlags::None, sizeof(u16), 6 * GPU_PARTICLE_MAX_COUNT_PER_EMITTER);
        m_particleStaticIndexBuffer = device->createBuffer(particleStaticIndexBufferDesc, "ParticleIndexBuffer", ibInitData);

        // TODO: share buffer with other CPU write buffers like InstanceData, MaterialInstanceData, etc..?
        BufferDesc particleDynamicVertexBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, sizeof(ParticleVertex), GPU_PARTICLE_MAX_COUNT_PER_FRAME);
        m_particleDynamicVertexBuffer = device->createBuffer(particleDynamicVertexBufferDesc, "ParticleVertexBuffer");
    }

    //--------------------------------------------------------------------------------------
    ParticleRenderDataUpdatePass::~ParticleRenderDataUpdatePass()
    {
        VG_SAFE_RELEASE(m_particleStaticIndexBuffer);
        VG_SAFE_RELEASE(m_particleDynamicVertexBuffer);
    }

    //--------------------------------------------------------------------------------------
    // Estimate returns a cost of 1 per instance to update
    //--------------------------------------------------------------------------------------
    core::u64 ParticleRenderDataUpdatePass::GetCostEstimate(const RenderPassContext & _renderContext) const
    {
        auto renderer = Renderer::get();
        const auto * cullingJobOutput = renderer->getSharedCullingJobOutput();
        VG_ASSERT(nullptr != cullingJobOutput);
        const auto & instances = cullingJobOutput->m_particleSystems;
        return cullingJobOutput->m_particleSystems.size();
    }

    //--------------------------------------------------------------------------------------
    // This is done on main thread before rendering jobs
    // For each particle system visible in this view we compute the offset used to store its geometry data
    //--------------------------------------------------------------------------------------
    void ParticleRenderDataUpdatePass::BeforeAll(const gfx::RenderPassContext & _renderContext)
    {
        VG_PROFILE_CPU("ParticleRenderData");

        const auto & particleSystemInstances = Renderer::get()->getSharedCullingJobOutput()->m_particleSystems;
                
        uint offset = 0;
        
        for (uint i = 0; i < particleSystemInstances.size(); ++i)
        {
            ParticleSystemInstance * particleSystem = VG_SAFE_STATIC_CAST(ParticleSystemInstance, particleSystemInstances[i]);

            const uint particleDataSize = particleSystem->GetGPURenderDataSize(); 
            particleSystem->setInstanceIndexBuffer(m_particleStaticIndexBuffer, 0);
            particleSystem->setInstanceVertexBuffer(m_particleDynamicVertexBuffer, offset);
            offset += particleDataSize;
            particleSystem->removeAtomicFlags(GraphicInstance::AtomicFlags::ParticleList);
        }
        
        m_mapSize = offset;
    }

    //--------------------------------------------------------------------------------------
    // This part is done in jobs that run in parallel
      //--------------------------------------------------------------------------------------
    void ParticleRenderDataUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        const auto & particleSystemInstances = Renderer::get()->getSharedCullingJobOutput()->m_particleSystems;
        size_t mapSize = m_mapSize;

        if (mapSize > 0)
        {
            const u8 * VG_RESTRICT data = (const u8 * VG_RESTRICT)_cmdList->map(m_particleDynamicVertexBuffer, mapSize).data;
            {
                for (uint i = 0; i < particleSystemInstances.size(); ++i)
                {
                    ParticleSystemInstance * particleSystem = VG_SAFE_STATIC_CAST(ParticleSystemInstance, particleSystemInstances[i]);
                    particleSystem->FillGPURenderData(data);                    
                }
            }
            _cmdList->unmap(m_particleDynamicVertexBuffer);
        }
    }
}
