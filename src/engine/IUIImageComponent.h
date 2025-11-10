#pragma once
#include "engine/Component/Renderer/UI/UIComponent.h"

namespace vg::engine
{
    class IUIImageComponent : public UIComponent
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IUIImageComponent, UIComponent);
    };
}