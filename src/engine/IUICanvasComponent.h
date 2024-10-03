#pragma once
#include "engine/Component/UI/UIComponent.h"

namespace vg::engine
{
    class IUICanvasComponent : public UIComponent
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IUICanvasComponent, UIComponent);

        virtual void SetViewIndex(core::uint _viewIndex) = 0;
    };
}
