#pragma once

#include "../PhysicsObjectComponent.h"

namespace vg::engine
{
    class MergeStaticCollidersComponent final : public PhysicsObjectComponent
    {
    public:
        VG_CLASS_DECL(MergeStaticCollidersComponent, PhysicsObjectComponent);

        MergeStaticCollidersComponent(const core::string & _name, IObject * _parent);
        ~MergeStaticCollidersComponent();

        void ToolUpdate(const Context & _context) final override;
    };
}