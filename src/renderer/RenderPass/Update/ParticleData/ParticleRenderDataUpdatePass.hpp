#include "ParticleRenderDataUpdatePass.h"
#include "renderer/Instance/ParticleSystem/ParticleSystemInstance.h"

#define GPU_PARTICLE_ALIGNMENT 16
#define GPU_PARTICLE_MAX_COUNT 4096

namespace vg::renderer
{
    static const uint s_SimulatedParticleDataBufferSize = sizeof(ParticleVertex) * GPU_PARTICLE_MAX_COUNT;

    //--------------------------------------------------------------------------------------
    ParticleRenderDataUpdatePass::ParticleRenderDataUpdatePass() :
        UpdatePass("ParticleDataUpdatePass")
    {
        auto * device = Device::get();

        // TODO: share buffer with other CPU write buffers like InstanceData, MaterialInstanceData, etc..?
        BufferDesc particleRenderDataBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, s_SimulatedParticleDataBufferSize);
        m_particleRenderDataBuffer = device->createBuffer(particleRenderDataBufferDesc, "ParticleRenderData");
    }

    //--------------------------------------------------------------------------------------
    ParticleRenderDataUpdatePass::~ParticleRenderDataUpdatePass()
    {
        VG_SAFE_RELEASE(m_particleRenderDataBuffer);
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
            particleSystem->setDynamicBuffer(m_particleRenderDataBuffer, offset);
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
            const u8 * VG_RESTRICT data = (const u8 * VG_RESTRICT)_cmdList->map(m_particleRenderDataBuffer, mapSize).data;
            {
                for (uint i = 0; i < particleSystemInstances.size(); ++i)
                {
                    ParticleSystemInstance * particleSystem = VG_SAFE_STATIC_CAST(ParticleSystemInstance, particleSystemInstances[i]);
                    particleSystem->FillGPURenderData(data);                    
                }
            }
            _cmdList->unmap(m_particleRenderDataBuffer);
        }
    }
}
