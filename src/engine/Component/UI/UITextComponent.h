#pragma once
#include "engine/Component/UI/UIComponent.h"

namespace vg::engine
{
    class UITextComponent : public UIComponent
    {
    public:
        VG_CLASS_DECL(UITextComponent, UIComponent);
        UITextComponent(const core::string & _name, core::IObject * _parent);
        ~UITextComponent();

        void Update(float _dt) final override;

    private:
        core::string m_text;
    };
}