#pragma once
#include "renderer/View/View.h"

namespace vg::core
{
    class IWorld;
}

namespace vg::gfx
{
    class IView;
}

namespace vg::renderer
{
    class LightInstance;
    class ShadowMapPass;

    class ShadowView final : public View
    {
    public:
        VG_CLASS_SUPER_CLASSNAME(ShadowView, View)
        ShadowView(LightInstance * _light, core::IWorld * _world, core::uint2 _resolution);
        ~ShadowView();

        bool                            IsLit               () const { return false; }
        bool                            IsUsingRayTracing   () const { return false; }

        void                            RegisterFrameGraph  (const gfx::RenderPassContext & _renderPassContext, gfx::FrameGraph & _frameGraph) override;

        const LightInstance *           getLight            () const { return m_light; }
        bool                            isUsingShadowMap    () const;
        core::string                    getShadowMapName    (const gfx::IView * _view) const;

    private:
        LightInstance *                 m_light         = nullptr;
        ShadowMapPass *                 m_shadowMapPass = nullptr;
    };
}