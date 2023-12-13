#pragma once

#include "core/Component/Component.h"
#include "gfx/IView.h"

namespace vg::core
{
    class IGameObject;
}

namespace vg::renderer
{
    class IView;
}

namespace vg::engine
{
    class CameraComponent : public core::Component
    {
    public:
        VG_CLASS_DECL(CameraComponent, core::Component);

                                                CameraComponent     (const core::string & _name, core::IObject * _parent);
                                                ~CameraComponent    ();

        void                                    Update              (float _dt) override;

        VG_INLINE void                          setViewTarget       (gfx::ViewTarget _target);
        VG_INLINE gfx::ViewTarget               getViewTarget       () const;
        VG_INLINE gfx::ViewID                   getViewID           () const;

        VG_INLINE float                         getFovY             () const;
        VG_INLINE float                         getNear             () const;
        VG_INLINE float                         getFar              () const;

        void                                    OnPropertyChanged   (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent);

    protected:
        gfx::ViewTarget                         m_target = gfx::ViewTarget::Game;
        core::u8                                m_index = 0;
        float                                   m_fovY;
        float                                   m_near;
        float                                   m_far;
    };
}

#if VG_ENABLE_INLINE
#include "CameraComponent.inl"
#endif