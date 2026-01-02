#pragma once
#include "renderer/View/View.h"

namespace vg::core
{
    class IWorld;
}

namespace vg::renderer
{
    class LightInstance;
    class ShadowMapPass;
    class IView;

    class ShadowView final : public View
    {
    public:
        VG_CLASS_SUPER_CLASSNAME(ShadowView, View)
        ShadowView(const LightInstance * _light, core::IWorld * _world, core::uint2 _resolution, View * _parentView = nullptr);
        ~ShadowView();

        bool                            IsLit               () const { return false; }
        bool                            IsUsingRayTracing   () const { return false; }

        void                            RegisterFrameGraph  (const gfx::RenderPassContext & _renderPassContext, gfx::FrameGraph & _frameGraph) override;

        const LightInstance *           getLight            () const { return m_light; }
        bool                            isUsingShadowMap    () const;
        core::string                    getShadowMapName    (const IView * _view) const;

    private:
        const LightInstance *           m_light         = nullptr;
        ShadowMapPass *                 m_shadowMapPass = nullptr;
        View *                          m_parentView    = nullptr;
    };
}