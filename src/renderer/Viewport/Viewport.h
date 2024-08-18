#pragma once

#include "gfx/IViewport.h"

namespace vg::gfx
{
    struct CreateViewportParams;
}

namespace vg::renderer
{
    class ViewConstantsUpdatePass;
    class ShadowView;
    class ViewGUI;

    //--------------------------------------------------------------------------------------
    // A Viewport is a collection of views rendered together
    //--------------------------------------------------------------------------------------
    class View : public gfx::IViewport
    {

    };
}