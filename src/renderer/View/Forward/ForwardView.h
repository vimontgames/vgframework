#pragma once
#include "renderer/View/View.h"

namespace vg::renderer
{
    class BackgroundPass;
    class ForwardPass;
    class TestRayTracingPass;
    class ComputePostProcessPass;
    class FinalBlitPass;
    

    class ForwardView : public View
    {
    public:
        const char *                getClassName    () const final { return "ForwardView"; }

                                    ForwardView     (const gfx::CreateViewParams & _params);
                                    ~ForwardView    ();


        void                        addToFrameGraph (gfx::FrameGraph & _frameGraph) override;

    private:
        BackgroundPass *            m_backgroundPass = nullptr;
        ForwardPass *               m_forwardPass = nullptr;
        TestRayTracingPass *        m_testRayTracingPass = nullptr;
        ComputePostProcessPass *    m_computePostProcessPass = nullptr;
        FinalBlitPass *             m_finalBlitPass = nullptr;
    };
}