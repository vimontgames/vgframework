#pragma once

#include "renderer/IParticleSystemInstance.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"

namespace vg::renderer
{
    class ParticleSystemInstance final : public IParticleSystemInstance
    {
    public:
        VG_CLASS_DECL(ParticleSystemInstance, IParticleSystemInstance);

                ParticleSystemInstance      (const core::string & _name, core::IObject * _parent);
                ~ParticleSystemInstance     ();

        void    SetModel                    (core::Lod _lod, core::IModel * _model) final override;
        bool    TryGetAABB                  (core::AABB & _aabb) const final override;
        bool    Cull                        (CullingResult * _cullingResult, View * _view) const final override;
        bool    OnUpdateRayTracing          (gfx::CommandList * _cmdList, View * _view, core::uint _index) final override;
        void    Draw                        (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;
    };
}

#if VG_ENABLE_INLINE
#include "ParticleSystemInstance.inl"
#endif