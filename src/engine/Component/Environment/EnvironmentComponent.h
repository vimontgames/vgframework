#pragma once

#include "core/Component/Component.h"
#include "engine/Resource/Texture/TextureResource.h"

namespace vg::engine
{
    class EnvironmentComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(EnvironmentComponent, core::Component);
        EnvironmentComponent(const core::string & _name, core::IObject * _parent);
        ~EnvironmentComponent();

        void Update(const Context & _context) final override;

    private:
        core::float4    m_environmentColor = core::float4(0.1f, 0.1f, 0.1f, 1.0f);
        bool            m_useEnvironmentCubemap = false;
        TextureResource m_environmentCubemap;
        float           m_irradiance = 1.0f;
        float           m_specularReflection = 1.0f;
    };
}