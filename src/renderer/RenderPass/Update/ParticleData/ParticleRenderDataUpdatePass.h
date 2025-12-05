#pragma once

#include "Renderer/RenderPass/Update/UpdatePass.h"
#include "core/Container/Span.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    class ParticleSystemInstance;

    //--------------------------------------------------------------------------------------
    // The purpose of this pass is to upload particle vertex buffer to the GPU from simulation data
    //--------------------------------------------------------------------------------------
    class ParticleRenderDataUpdatePass final : public UpdatePass
    {
    public:
        const char * GetClassName() const final { return "ParticleRenderDataUpdatePass"; }

        ParticleRenderDataUpdatePass();
        ~ParticleRenderDataUpdatePass();
        
        core::u64           GetCostEstimate             (const gfx::RenderPassContext & _renderContext) const final override;
        void                BeforeAll                   (const gfx::RenderPassContext & _renderContext) final override;
        void                BeforeRender                (const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;

        const gfx::Buffer * getIndexBuffer              () const { return m_particleStaticIndexBuffer; }
        const gfx::Buffer * getVertexBuffer             () const { return m_particleDynamicVertexBuffer; }

        core::span<ParticleSystemInstance *> getParticleSystemInstances() const;

    private:
        gfx::Buffer *       m_particleStaticIndexBuffer     = nullptr;
        gfx::Buffer *       m_particleDynamicVertexBuffer   = nullptr;
        core::uint          m_mapSize = 0;
    };
}