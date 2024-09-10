#pragma once

#include "core/Component/Component.h"
#include "core/Object/ObjectHandle.h"

namespace vg::engine
{
    class HUDComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(HUDComponent, core::Component);

        HUDComponent(const core::string & _name, core::IObject * _parent);
        ~HUDComponent();

        void            Update(const Context & _context) final override;

    protected:
        vg::core::ObjectHandle  m_fpsText;
        vg::core::ObjectHandle  m_cpuText;
        vg::core::ObjectHandle  m_gpuText;        
    };
}