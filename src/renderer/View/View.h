#pragma once

#include "gfx/IView.h"
#include "gfx/Resource/Texture.h"
#include "renderer/Job/Culling/ViewCullingJob.h"

namespace vg::core
{
    class GameObject;
    class Job;
}

namespace vg::gfx
{
    class FrameGraph;
}

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Base class for user views.
    // The 'IView' interface is declared in graphics::driver so that the Framegraph can  
    // reference a view, but the 'View' base class is defined in graphics::renderer.
    //--------------------------------------------------------------------------------------

    class View : public gfx::IView
    {
    public:
        const char *                        getClassName        () const override { return "View"; }

                                            View                (const gfx::CreateViewParams & _params);
                                            ~View               ();

        void                                SetupCamera         (const core::float4x4 & _viewInv, core::float2 _nearFar, float _fovY) override;

        const core::float4x4 &              GetViewInvMatrix    () const override;
        core::float2                        GetCameraNearFar    () const override;
        float                               GetCameraFovY       () const override;

        void                                SetUniverse         (core::IUniverse* _universe) override;
        core::IUniverse *                   GetUniverse         () const override;

        void                                SetSize             (core::uint2 _size) override;
        core::uint2                         GetSize             () const override;

        void                                SetOffset           (core::int2) override;
        core::int2                          GetOffset           () const override;

        void                                SetRenderTarget     (gfx::ITexture * _renderTarget) override;
        gfx::ITexture *                  GetRenderTarget     () const override;

        void                                SetViewID           (gfx::ViewID _viewID) override;
        gfx::ViewID                      GetViewID           () const override;

        void                                SetActive           (bool _active) override;
        bool                                IsActive            () const override;

        virtual void                        AddToFrameGraph     (gfx::FrameGraph & _frameGraph) {} // TODO: implement "MainView" using 'AddToFrameGraph'?

        core::u32                           release             () override;

        VG_INLINE core::IUniverse *         getUniverse         () const;

        VG_INLINE void                      setSize             (core::uint2 _size);
        VG_INLINE core::uint2               getSize             () const;

        VG_INLINE void                      setOffset           (core::int2 _offset);
        VG_INLINE core::int2                getOffset           () const;

        VG_INLINE const core::float4x4 &    getViewProjMatrix   () const;
        VG_INLINE const core::float4x4 &    getViewInvMatrix    () const;
        VG_INLINE core::float2              getCameraNearFar    () const;
        VG_INLINE float                     getCameraFovY       () const;

        VG_INLINE void                      setViewID           (gfx::ViewID _viewID);
        VG_INLINE gfx::ViewID            getViewID           () const;

        VG_INLINE void                      setRenderTarget     (gfx::Texture * _renderTarget);
        VG_INLINE gfx::Texture *         getRenderTarget     () const;

        VG_INLINE core::Job *               getCullingJob       () const;

    private:
        static core::float4x4               setPerspectiveProjectionRH(float _fov, float _ar, float _near, float _far);

    private:
        gfx::ViewID                      m_viewID = gfx::ViewID((gfx::ViewType)-1, -1);
        gfx::Texture *                   m_renderTarget = nullptr;   // Assume backbuffer if nullptr
        core::uint2                         m_size = core::uint2(0, 0);
        core::int2                          m_offset = core::int2(0, 0);
        core::float4x4                      m_viewInv = core::float4x4::identity();
        core::float4x4                      m_viewProj = core::float4x4::identity();
        core::IUniverse *                   m_cameraUniverse = nullptr;
        core::float2                        m_cameraNearFar;
        float                               m_cameraFovY;
        bool                                m_active = false;
        ViewCullingJob *                    m_cullingJob = nullptr;

    public:
        ViewCullingJobOutput                m_cullingJobResult;
    };
}

#if VG_ENABLE_INLINE
#include "View.inl"
#endif