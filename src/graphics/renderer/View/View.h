#pragma once

#include "graphics/renderer/IView.h"

namespace vg::core
{
    class Sector;
}

namespace vg::graphics::renderer
{
    class View : public IView
    {
    public:
        const char * getClassName() const final { return "View"; }

        View(const CreateViewParams & _params);
        ~View();

        void SetViewInvMatrix(const core::float4x4 & _viewInv) override;
        const core::float4x4 & GetViewInvMatrix() const override;
        VG_INLINE const core::float4x4 & getViewInvMatrix() const;

        void SetCameraSector(core::ISector * _cameraSector) override;
        core::ISector * GetCameraSector() const override;
        VG_INLINE core::Sector * getCameraSector() const;

    private:
        core::float4x4  m_viewInv = core::float4x4::identity();
        core::Sector *  m_cameraSector = nullptr;
    };
}

#if VG_ENABLE_INLINE
#include "View.inl"
#endif