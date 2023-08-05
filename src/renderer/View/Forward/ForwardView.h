#pragma once
#include "renderer/View/View.h"

namespace vg::renderer
{
    class BackgroundPass;
    class ForwardPass;
    class PostProcessPass;

    class ForwardView : public View
    {
    public:
        const char *        getClassName    () const final { return "ForwardView"; }

                            ForwardView     (const gfx::CreateViewParams & _params);
                            ~ForwardView    ();


        void                AddToFrameGraph (gfx::FrameGraph & _frameGraph) override;

    private:
        BackgroundPass *    m_backgroundPass = nullptr;
        ForwardPass *       m_forwardPass = nullptr;
        PostProcessPass *   m_postProcessPass = nullptr;
    };
}