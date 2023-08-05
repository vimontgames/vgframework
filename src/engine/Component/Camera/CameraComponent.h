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

        void                                    Update              (double _dt) override;

        VG_INLINE void                          setViewType         (gfx::ViewType _viewType);
        VG_INLINE gfx::ViewType    getViewType         () const;
        VG_INLINE gfx::ViewID      getViewID           () const;

        VG_INLINE float                         getFovY             () const;
        VG_INLINE float                         getNear             () const;
        VG_INLINE float                         getFar              () const;

    protected:
        gfx::ViewType              m_ViewType = gfx::ViewType::Game;
        core::u8                                m_ViewIndex = 0;
        float                                   m_fovY;
        float                                   m_near;
        float                                   m_far;
    };
}

#if VG_ENABLE_INLINE
#include "CameraComponent.inl"
#endif