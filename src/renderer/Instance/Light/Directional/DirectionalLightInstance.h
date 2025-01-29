#pragma once
#include "renderer/Instance/Light/LightInstance.h"

namespace vg::renderer
{
    class DirectionalLightDesc final : public LightDesc
    {
    public:
        VG_CLASS_DECL(DirectionalLightDesc, LightDesc);

        DirectionalLightDesc(const core::string & _name, core::IObject * _parent);
        ~DirectionalLightDesc();

        LightType   GetLightType() const final override { return LightType::Directional; }
    };

    class DirectionalLightInstance : public LightInstance
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(DirectionalLightInstance, LightInstance);

        DirectionalLightInstance(const DirectionalLightDesc * _directionalLightDesc);
        ~DirectionalLightInstance();

        bool            TryGetAABB(AABB & _aabb) const final override;
        bool            Cull(CullingResult * _cullingResult, View * _view) const final override;
        void            Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;

        LightType       GetLightType() const final override { return LightType::Directional; }
    };
}