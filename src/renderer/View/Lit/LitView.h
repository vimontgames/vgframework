#pragma once

#include "renderer/View/View.h"

namespace vg::renderer
{
    class BackgroundPass;
    class DepthPrePass;
    class ForwardOpaquePass;
    class ForwardTransparentPass;
    class DeferredOpaquePass;
    class ComputeDeferredLightingPass;
    class ResolveDeferredMSAAPass;
    class LinearizeDepthPass;
    class EditorPass;
    class RayTracingDebugPass;
    class ComputePostProcessPass;
    class FinalBlitPass;
    class TLASUpdatePass;

    class LitView final : public View
    {
    public:
        using super = View;

        const char *                    GetClassName        () const final override { return "ForwardView"; }

                                        LitView             (const CreateViewParams & _params);
                                        ~LitView            ();

        bool                            IsLit               () const { return true; }
        bool                            IsUsingRayTracing   () const final override;


        void                            RegisterFrameGraph  (const gfx::RenderPassContext & _renderPassContext, gfx::FrameGraph & _frameGraph) override;

    private:
        BackgroundPass *                m_backgroundPass            = nullptr;
        DepthPrePass *                  m_depthPrePass              = nullptr;
        ForwardOpaquePass *             m_forwardOpaquePass         = nullptr;
        ForwardTransparentPass *        m_forwardTransparentPass    = nullptr;
        DeferredOpaquePass *            m_deferredOpaquePass        = nullptr;
        ComputeDeferredLightingPass *   m_deferredLightingPass      = nullptr;
        ResolveDeferredMSAAPass *       m_resolveDeferredMSAAPass   = nullptr;
        LinearizeDepthPass *            m_linearizeDepthPass        = nullptr;
        EditorPass *                    m_editorPass                = nullptr;
        TLASUpdatePass *                m_TLASUpdatePass            = nullptr;
        ComputePostProcessPass *        m_computePostProcessPass    = nullptr;
        FinalBlitPass *                 m_finalBlitPass             = nullptr;
    };
}