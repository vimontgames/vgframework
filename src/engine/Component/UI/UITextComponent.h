#pragma once
#include "engine/IUITextComponent.h"

namespace vg::engine
{
    class UITextComponent : public IUITextComponent
    {
    public:
        VG_CLASS_DECL(UITextComponent, IUITextComponent);
        UITextComponent(const core::string & _name, core::IObject * _parent);
        ~UITextComponent();

        void    SetText    (const core::string & _text) final override;
        void    Update      (float _dt) final override;

    private:
        core::string m_text;
    };
}