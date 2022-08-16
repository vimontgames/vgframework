#pragma once

#include "graphics/renderer/IView.h"

namespace vg::core
{
    class GameObject;
}

namespace vg::graphics::renderer
{
    class View : public IView
    {
    public:
        const char *                        getClassName        () const final { return "View"; }

                                            View                (const CreateViewParams & _params);
                                            ~View               ();

        void                                SetupCamera         (const core::float4x4 & _viewInv, core::float2 _nearFar, float _fovY) override;

        const core::float4x4 &              GetViewInvMatrix    () const override;
        core::float2                        GetCameraNearFar    () const override;
        float                               GetCameraFovY       () const override;

        void                                SetCameraSector     (core::IGameObject * _cameraSector) override;
        core::IGameObject *                 GetCameraSector     () const override;

        VG_INLINE const core::float4x4 &    getViewInvMatrix    () const;
        VG_INLINE core::float2              getCameraNearFar    () const;
        VG_INLINE float                     getCameraFovY       () const;
        VG_INLINE core::GameObject *        getCameraSector     () const;

    private:
        core::float4x4                      m_viewInv = core::float4x4::identity();
        core::GameObject *                  m_cameraSector = nullptr;
        core::float2                        m_cameraNearFar;
        float                               m_cameraFovY;
    };
}

#if VG_ENABLE_INLINE
#include "View.inl"
#endif