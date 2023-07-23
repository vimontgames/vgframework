#pragma once
#include "graphics/Renderer/View/View.h"

namespace vg::graphics::renderer
{
    class BackgroundPass;
    class ForwardPass;
    class PostProcessPass;

    class ForwardView : public View
    {
    public:
        const char *        getClassName    () const final { return "ForwardView"; }

                            ForwardView     (const driver::CreateViewParams & _params);
                            ~ForwardView    ();


        void                AddToFrameGraph (driver::FrameGraph & _frameGraph) override;

    private:
        BackgroundPass *    m_backgroundPass = nullptr;
        ForwardPass *       m_forwardPass = nullptr;
        PostProcessPass *   m_postProcessPass = nullptr;
    };
}