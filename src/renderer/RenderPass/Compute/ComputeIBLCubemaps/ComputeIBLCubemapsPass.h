#pragma once

#include "Renderer/RenderPass/Compute/ComputePass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class ComputeIBLCubemapsPass : public ComputePass
    {
    public:
        const char * GetClassName() const final { return "ComputeIrradianceCubemapPass"; }

        ComputeIBLCubemapsPass();
        ~ComputeIBLCubemapsPass();

        void Setup(const gfx::RenderPassContext & _renderContext) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

        void add(gfx::Texture * _sourceEnvironmentCubemap, gfx::Texture * _destinationIrradianceCubemap, gfx::Texture * _destinationSpecularReflectionCubemap);

    private:
        void  computeIrradianceCubemap(gfx::CommandList * _cmdList, const gfx::Texture * _sourceEnvironmentCubemap, gfx::Texture * _destinationIrradianceCubemap) const;
        void  computeSpecularReflectionCubemap(gfx::CommandList * _cmdList, const gfx::Texture * _sourceEnvironmentCubemap, gfx::Texture * _destinationSpecularReflectionCubemap) const;

    private:
        gfx::RootSignatureHandle    m_computeIrradianceCubemapRootSignature;
        gfx::ComputeShaderKey       m_computeIrradianceCubemapShaderKey;
        gfx::ComputeShaderKey       m_computeSpecularReflectionShaderKey;

        struct ComputeIBLCubemapsData
        {
            const gfx::Texture * m_sourceEnvironmentCubemap = nullptr;
            gfx::Texture * m_destinationIrradianceCubemap = nullptr;
            gfx::Texture * m_destinationSpecularReflectionCubemap = nullptr;
        };  

        mutable core::vector<ComputeIBLCubemapsData> m_computeIBLCubemaps;
    };
}