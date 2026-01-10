#pragma once

#include "core/Component/Component.h"
#include "engine/Resource/Texture/TextureResource.h"

namespace vg::engine
{
    vg_enum_class(vg::engine, DummyType, core::u32,
        Box     = 0x00000001,
        Sphere  = 0x00000002
    );

    class DummyComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(DummyComponent, core::Component);
        DummyComponent(const core::string & _name, core::IObject * _parent);
        ~DummyComponent();

        void ToolUpdate(const Context & _context) final override;

    private:
        DummyType m_dummyType = DummyType::Box;
    };
}