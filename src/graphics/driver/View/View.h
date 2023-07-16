#pragma once

#include "graphics/driver/IView.h"
#include "graphics/driver/Resource/Texture.h"

namespace vg::core
{
    class GameObject;
}

namespace vg::graphics::driver
{
    class FrameGraph;

    //--------------------------------------------------------------------------------------
    // Base class for user views
    //--------------------------------------------------------------------------------------

    class View : public IView
    {
    public:
        const char *                        getClassName        () const override { return "View"; }

                                            View                (const CreateViewParams & _params);
                                            ~View               ();

        void                                SetupCamera         (const core::float4x4 & _viewInv, core::float2 _nearFar, float _fovY) override;

        const core::float4x4 &              GetViewInvMatrix    () const override;
        core::float2                        GetCameraNearFar    () const override;
        float                               GetCameraFovY       () const override;

        void                                SetCameraSector     (core::IGameObject * _cameraSector) override;
        core::IGameObject *                 GetCameraSector     () const override;

        void                                SetUniverse         (core::IUniverse* _universe) override;
        core::IUniverse *                   GetUniverse         () const override;

        void                                SetSize             (core::uint2 _size) override;
        core::uint2                         GetSize             () const override;

        void                                SetOffset           (core::int2) override;
        core::int2                          GetOffset           () const override;

        void                                SetRenderTarget     (driver::ITexture * _renderTarget) override;
        driver::ITexture *                  GetRenderTarget     () const override;

        void                                SetViewID           (ViewID _viewID) override;
        ViewID                              GetViewID           () const override;

        virtual void                        AddToFrameGraph     (FrameGraph & _frameGraph) {} // TODO: implement "MainView" using 'AddToFrameGraph'?

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

        VG_INLINE void                      setViewID           (ViewID _viewID);
        VG_INLINE ViewID                    getViewID           () const;

        VG_INLINE void                      setRenderTarget     (driver::Texture * _renderTarget);
        VG_INLINE driver::Texture *         getRenderTarget     () const;

    private:
        ViewID                              m_viewID = -1;
        driver::Texture *                   m_renderTarget = nullptr;   // Assume backbuffer if nullptr
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