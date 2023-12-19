#pragma once

#include "gfx/Shader/RayTracingShaderKey.h"
#include "Renderer/RenderPass/RayTracing/RayTracingPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class TestRayTracingPass final : public RayTracingPass
    {
    public:
        const char * getClassName() const final { return "TestRayTracingPass"; }

        TestRayTracingPass();
        ~TestRayTracingPass();

        void Setup(const gfx::RenderPassContext & _renderContext, double _dt) final override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const final override;

    private:
        gfx::RootSignatureHandle    m_testRayTracingGlobalRootSignatureHandle;
        gfx::RayTracingShaderKey    m_testRayTracingShaderKey;
    };
}