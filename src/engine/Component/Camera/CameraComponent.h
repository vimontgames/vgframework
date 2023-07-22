#pragma once

#include "core/Component/Component.h"
#include "graphics/driver/IView.h"

namespace vg::core
{
    class IGameObject;
}

namespace vg::graphics::renderer
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

        VG_INLINE void                          setViewType         (graphics::driver::ViewType _viewType);
        VG_INLINE graphics::driver::ViewType    getViewType         () const;
        VG_INLINE graphics::driver::ViewID      getViewID           () const;

        VG_INLINE float                         getFovY             () const;
        VG_INLINE float                         getNear             () const;
        VG_INLINE float                         getFar              () const;

    protected:
        graphics::driver::ViewType              m_ViewType = graphics::driver::ViewType::Game;
        core::u8                                m_ViewIndex = 0;
        float                                   m_fovY;
        float                                   m_near;
        float                                   m_far;
    };
}

#if VG_ENABLE_INLINE
#include "CameraComponent.inl"
#endif