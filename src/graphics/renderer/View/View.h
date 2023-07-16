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

        void                                SetupCamera         (const core::float4x4 & _viewInv, core::float2 _nearFar, float _fovY) final;

        const core::float4x4 &              GetViewInvMatrix    () const final;
        core::float2                        GetCameraNearFar    () const final;
        float                               GetCameraFovY       () const final;

        void                                SetCameraSector     (core::IGameObject * _cameraSector) final;
        core::IGameObject *                 GetCameraSector     () const final;

        void                                SetUniverse         (core::IUniverse* _universe) final;
        core::IUniverse *                   GetUniverse         () const final;

        void                                SetSize             (core::uint2 _size) final override; 
        core::uint2                         GetSize             () const final override;

        void                                SetOffset           (core::int2) final override;
        core::int2                          GetOffset           () const final override;

        void                                SetRenderTarget     (driver::ITexture * _renderTarget) final override;
        driver::ITexture *                  GetRenderTarget     () const final override;

        core::u32                           release             () override;

        VG_INLINE core::IUniverse *         getUniverse         () const;

        VG_INLINE void                      setSize             (core::uint2 _size);
        VG_INLINE core::uint2               getSize             () const;

        VG_INLINE void                      setOffset           (core::int2 _offset);
        VG_INLINE core::int2                getOffset           () const;

        VG_INLINE const core::float4x4 &    getViewInvMatrix    () const;
        VG_INLINE core::float2              getCameraNearFar    () const;
        VG_INLINE float                     getCameraFovY       () const;
        VG_INLINE core::GameObject *        getCameraSector     () const;

    private:
        driver::ITexture *                  m_renderTarget = nullptr;   // Assume backbuffer if nullptr
        core::uint2                         m_size = core::uint2(0, 0);
        core::int2                          m_offset = core::int2(0, 0);
        core::float4x4                      m_viewInv = core::float4x4::identity();
        core::IUniverse *                   m_cameraUniverse = nullptr;
        core::GameObject *                  m_cameraSector = nullptr;
        core::float2                        m_cameraNearFar;
        float                               m_cameraFovY;
    };
}

#if VG_ENABLE_INLINE
#include "View.inl"
#endif