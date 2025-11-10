#pragma once
#include "engine/Component/Renderer/UI/UIComponent.h"

namespace vg::engine
{
    class IUITextComponent : public UIComponent
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IUITextComponent, UIComponent);

        virtual void SetText(const core::string & _text) = 0;
    };
}