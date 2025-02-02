#pragma once

#include "engine/ICameraComponent.h"
#include "engine/Resource/Lens/LensResource.h"
#include "renderer/ICameraSettings.h"

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
    using PickingID = core::uint;
}

namespace vg::engine
{
    class CameraComponent final : public ICameraComponent
    {
    public:
        VG_CLASS_DECL(CameraComponent, ICameraComponent);

                                                CameraComponent             (const core::string & _name, core::IObject * _parent);
                                                ~CameraComponent            ();

        void                                    OnLoad                      () final override;
        void                                    OnPropertyChanged           (vg::core::IObject * _object, const vg::core::IProperty & _prop, bool _notifyParent) final override;
        void                                    Update                      (const Context & _context) final override;

        void                                    SetViewportOffsetAndScale   (core::float2 _offset, const core::float2 & _scale) final override;

        VG_INLINE void                          setViewportTarget           (gfx::ViewportTarget _target);
        VG_INLINE gfx::ViewportTarget           getViewportTarget           () const;
        gfx::ViewID                             getViewID                   () const;

        VG_INLINE float                         getFocalLength              () const;
        VG_INLINE float                         getNear                     () const;
        VG_INLINE float                         getFar                      () const;

        void                                    updateLensConstraints       ();

    protected:
        gfx::ViewportTarget                     m_target;
        gfx::ViewportIndex                      m_viewportIndex;
        gfx::ViewIndex                          m_viewIndex;
        renderer::ICameraSettings *             m_cameraSettings = nullptr;
        core::float2                            m_viewportOffset;
        core::float2                            m_viewportScale;
        renderer::PickingID                     m_pickingID = 0;
    };
}

#if VG_ENABLE_INLINE
#include "CameraComponent.inl"
#endif