#pragma once

#include "renderer/View/View.h"

namespace vg::renderer
{
    class BackgroundPass;
    class DepthOnlyPass;
    class ForwardPass;
    class EditorPass;
    class TestRayTracingPass;
    class ComputePostProcessPass;
    class FinalBlitPass;
    class TLASUpdatePass;

    class ForwardView final : public View
    {
    public:
        using super = View;

        const char *                getClassName        () const final override { return "ForwardView"; }

                                    ForwardView         (const gfx::CreateViewParams & _params);
                                    ~ForwardView        ();

        bool                        IsUsingRayTracing   () const final override;

        void                        RegisterFrameGraph (const gfx::RenderPassContext & _renderPassContext, gfx::FrameGraph & _frameGraph) override;

    private:
        BackgroundPass *            m_backgroundPass            = nullptr;
        DepthOnlyPass *             m_depthPrePass              = nullptr;
        ForwardPass *               m_forwardPass               = nullptr;
        EditorPass *                m_editorPass                = nullptr;
        TLASUpdatePass *            m_TLASUpdatePass            = nullptr;
        TestRayTracingPass *        m_testRayTracingPass        = nullptr;
        ComputePostProcessPass *    m_computePostProcessPass    = nullptr;
        FinalBlitPass *             m_finalBlitPass             = nullptr;
    };
}