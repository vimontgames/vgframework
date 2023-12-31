#pragma once
#include "renderer/Instance/Light/LightInstance.h"

namespace vg::renderer
{
    class OmniLightDesc final : public LightDesc
    {
    public:
        VG_CLASS_DECL(OmniLightDesc, LightDesc);
        
        OmniLightDesc(const core::string & _name, core::IObject * _parent);
        ~OmniLightDesc();

        LightType   GetLightType() const final override { return LightType::Omni; }
        
        bool  m_useMaxRadius = false;
        float m_maxRadius = 1.0f;
    };

    class OmniLightInstance : public LightInstance
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(OmniLightInstance, LightInstance);

        OmniLightInstance(const OmniLightDesc * _omniLightDesc);
        ~OmniLightInstance();

        bool            GetAABB             (AABB & _aabb) const final override;
        bool            Cull                (const Frustum & _frustum, CullingResult * _cullingResult) final override;
        void            Draw                (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;

        LightType       GetLightType        () const final override { return LightType::Omni; }

        float           getMaxRadius() const;

    private:
        float m_maxRadius = -1.0f;
    };
}