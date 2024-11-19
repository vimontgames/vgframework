#pragma once

#include "engine/ICameraComponent.h"
#include "gfx/IView.h"
#include "renderer/PBR/PhysicalCameraSettings.h"

namespace vg::core
{
    class IGameObject;
}

namespace vg::gfx
{
    enum class ViewportTarget : core::u8;
    using ViewportIndex = core::u8;
    using ViewIndex = core::u8;
}

namespace vg::renderer
{
    class IView;
}

namespace vg::engine
{
    class CameraComponent : public ICameraComponent
    {
    public:
        VG_CLASS_DECL(CameraComponent, ICameraComponent);

                                                CameraComponent             (const core::string & _name, core::IObject * _parent);
                                                ~CameraComponent            ();

        void                                    Update                      (const Context & _context) override;

        void                                    SetViewportOffsetAndScale   (core::float2 _offset, const core::float2 & _scale) final override;

        VG_INLINE void                          setViewportTarget           (gfx::ViewportTarget _target);
        VG_INLINE gfx::ViewportTarget           getViewportTarget           () const;
        gfx::ViewID                             getViewID                   () const;

        VG_INLINE float                         getNear                     () const;
        VG_INLINE float                         getFar                      () const;

    protected:
        gfx::ViewportTarget                     m_target;
        gfx::ViewportIndex                      m_viewportIndex;
        gfx::ViewIndex                          m_viewIndex;
        renderer::PhysicalCameraSettings        m_physicalCameraSettings;
        core::float2                            m_viewportOffset;
        core::float2                            m_viewportScale;
        core::float2                            m_previousViewportScale = core::float2(0, 0);
    };
}

#if VG_ENABLE_INLINE
#include "CameraComponent.inl"
#endif