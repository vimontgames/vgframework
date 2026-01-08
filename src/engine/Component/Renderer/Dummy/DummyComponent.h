#pragma once

#include "core/Component/Component.h"
#include "engine/Resource/Texture/TextureResource.h"

namespace vg::engine
{
    class DummyComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(DummyComponent, core::Component);
        DummyComponent(const core::string & _name, core::IObject * _parent);
        ~DummyComponent();

        void ToolUpdate(const Context & _context) final override;
    };
}