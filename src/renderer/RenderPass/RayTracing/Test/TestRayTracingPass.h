#pragma once

#include "Renderer/RenderPass/RayTracing/RayTracingPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class TestRayTracingPass : public RayTracingPass
    {
    public:
        const char * getClassName() const final { return "TestRayTracingPass"; }

        TestRayTracingPass();
        ~TestRayTracingPass();

        void Setup(const gfx::RenderPassContext & _renderContext, double _dt) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        gfx::RootSignatureHandle     m_testRayTracingGlobalRootSignatureHandle;
    };
}