#pragma once
#include "renderer/ICameraInstance.h"

namespace vg::renderer
{
    class CameraInstance final  : public ICameraInstance
    {
    public:
        VG_CLASS_DECL(CameraInstance, ICameraInstance);

        CameraInstance(const core::string & _name, core::IObject * _parent);
        ~CameraInstance();

        bool    Cull                    (const CullingOptions & _cullingOptions, CullingResult * _cullingResult) final override;
        void    Draw                    (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const final override;
    };
}

#if VG_ENABLE_INLINE
#include "CameraInstance.inl"
#endif