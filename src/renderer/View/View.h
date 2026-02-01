#pragma once

#include "core/Scheduler/Mutex.h"
#include "gfx/Resource/Texture.h"
#include "gfx/FrameGraph/FrameGraph_consts.h"
#include "renderer/IView.h"
#include "renderer/Job/Culling/ViewCullingJob.h"
#include "Shaders/system/picking.hlsli"    
#include "Frustum.h"

namespace vg::core
{
    class GameObject;
    class Job;
}

namespace vg::gfx
{
    struct CreateViewParams;
    class RenderPassContext;
    class FrameGraph;
    class Buffer;
    class TLAS;
    class IUIRenderer;
}

namespace vg::renderer
{
    class ViewConstantsUpdatePass;
    class ShadowView;
    class UIRenderer;
    enum class TLASMode : core::u8;

    //--------------------------------------------------------------------------------------
    // Base class for user views.
    // The 'IView' interface is declared in graphics::driver so that the Framegraph can  
    // reference a view, but the 'View' base class is defined in graphics::renderer.
    //--------------------------------------------------------------------------------------

    class View : public IView
    {
    public:

        const char *                        GetClassName                        () const override { return "View"; }

                                            View                                (const CreateViewParams & _params);
                                            ~View                               ();

        void                                SetupPerspectiveCamera              (const core::float4x4 & _cameraWorldMatrix, core::float2 _nearFar, float _fovY, core::float2 _viewportOffset, core::float2 _viewportScale, const core::IGameObject * _cameraGO = nullptr, PickingID _pickingID = 0) final override;
        void                                SetupOrthographicCamera             (const core::float4x4 & _cameraWorldMatrix, core::uint2 _size, core::float2 _nearFar, const core::IGameObject * _cameraGO = nullptr, PickingID _pickingID = 0) final override;
        void                                SetupOrthographicCameraOffCenter    (const core::float4x4 & _cameraWorldMatrix, float _left, float _right, float _bottom, float _top, float _nearZ, float _farZ, const core::IGameObject * _cameraGO = nullptr, PickingID _pickingID = 0) final override;
        void                                SetupPhysicalCamera                 (const core::float4x4 & _cameraWorldMatrix, const ICameraSettings * _cameraSettings, core::float2 _viewportOffset, core::float2 _viewportScale, const core::IGameObject * _cameraGO = nullptr, PickingID _pickingID = 0) final override;

        void                                SetFlags                            (ViewFlags _flagsToSet, ViewFlags _flagsToRemove = (ViewFlags)0) override;
        ViewFlags                           GetFlags                            () const override;

        const core::float4x4 &              GetViewInvMatrix                    () const override;
        const core::float4x4 &              GetViewMatrix                       () const override;
        const core::float4x4 &              GetProjectionMatrix                 () const override;
        core::float2                        GetCameraNearFar                    () const override;
        float                               GetCameraFovY                       () const override;

        void                                SetWorld                            (core::IWorld * _world) override;
        core::IWorld *                      GetWorld                            () const override;

        void                                SetRenderTargetSize                 (core::uint2 _size) override;
        core::uint2                         GetRenderTargetSize                 () const override;

        core::uint2                         GetSize                             () const final override;
        core::int2                          GetOffset                           () const final override;

        core::float2                        GetViewportOffset                   () const final override;
        core::float2                        GetViewportScale                    () const final override;
        IViewport *                         GetViewport                         () const final override;

        void                                SetRenderTarget                     (gfx::ITexture * _renderTarget) override;
        gfx::ITexture *                     GetRenderTarget                     () const override;

        void                                SetViewID                           (gfx::ViewID _viewID) override;
        gfx::ViewID                         GetViewID                           () const override;

        void                                SetFocused                          (bool _active) override;
        bool                                IsFocused                           () const override;

        void                                SetVisible                          (bool _visible) override;
        bool                                IsVisible                           () const override;

        void                                SetRender                           (bool _render) override;
        bool                                IsRender                            () const override;

        void                                SetMouseOffset                      (const core::uint2 & _mouseOffset) override;
        core::int2                          GetRelativeMousePos                 () const override;
        bool                                IsMouseOverView                     () const final override;

        const core::string                  GetFrameGraphID                     (const core::string & _name) const final override;

        bool                                IsToolmode                          () const override;
        bool                                IsOrtho                             () const override;
        bool                                IsUsingRayTracing                   () const override;
        bool                                IsLit                               () const override;
        bool                                IsComputePostProcessNeeded          () const override;
        bool                                IsOutlinePassNeeded                 () const override;

        void                                setTLAS                             (gfx::TLAS * _tlas, TLASMode _mode = (TLASMode) - 1);
        gfx::TLAS *                         getTLAS                             () const;
        TLASMode                            getTLASMode                         () const;
        gfx::BindlessTLASHandle             getTLASHandle                       () const;

        void                                SetPickingData                      (const PickingData & _pickingData) override;
        virtual const PickingHit &          GetPickingHit                       (core::uint _index) const override;
        virtual core::uint                  GetPickingHitCount                  () const override;
        virtual core::uint                  GetPickingRequestedHitCount         () const override;
        const PickingHit &                  GetPickingClosestHit                () const override;
        void                                AddPickingHit                       (const PickingHit & _hit) override;

        ViewCullingStats                    GetViewCullingStats                 () const final override;
        IUIRenderer *                       GetUIRenderer                       () const final override;
        const ICameraSettings *             GetCameraSettings                   () const final override;

        const gfx::Buffer *                 GetParticleVertexBuffer             () const override { return nullptr; }
        
        core::vector<gfx::FrameGraphResourceID>  getShadowMaps                  () const;

        VG_INLINE const Frustum &           getCameraFrustum                    () const;

        virtual void                        RegisterFrameGraph                  (const gfx::RenderPassContext & _rc, gfx::FrameGraph & _frameGraph);

        const ViewCullingJobOutput &        getCullingJobResult                 () const;

        VG_INLINE core::IWorld *            getWorld                            () const;
        VG_INLINE const core::float4x4 &    getViewProjMatrix                   () const;
        VG_INLINE const core::float4x4 &    getViewMatrix                       () const;
        VG_INLINE const core::float4x4 &    getViewInvMatrix                    () const;
        VG_INLINE const core::float4x4 &    getProjMatrix                       () const;
        VG_INLINE const core::float4x4 &    getProjInvMatrix                    () const;

        VG_INLINE core::float2              getCameraNearFar                    () const;
        VG_INLINE float                     getCameraFovY                       () const;

        VG_INLINE void                      setViewID                           (gfx::ViewID _viewID);
        VG_INLINE gfx::ViewID               getViewID                           () const;

        VG_INLINE void                      setRenderTarget                     (gfx::Texture * _renderTarget);
        VG_INLINE gfx::Texture *            getRenderTarget                     () const;

        VG_INLINE core::float2              getViewportOffset                   () const;
        VG_INLINE core::float2              getViewportScale                    () const;

        core::vector<ViewCullingSplitJob *> getCullingSplitJobs                 ();
        VG_INLINE ViewCullingFinalJob *     getCullingFinalJob                  () const;
        bool                                startJobs                           ();

        VG_INLINE void                      setFlags                            (ViewFlags _flagsToSet, ViewFlags _flagsToRemove = (ViewFlags)0);
        VG_INLINE void                      setFlag                             (ViewFlags _flag, bool _value);
        VG_INLINE ViewFlags                 getFlags                            () const;
        VG_INLINE bool                      testFlag                            (ViewFlags _flag) const;

        bool                                isToolmode                          () const;

        void                                addShadowView                       (ShadowView * _shadowView);
        const core::vector<ShadowView *> &  getShadowViews                      () const { return m_shadowViews; }
        VG_INLINE void                      setIsAdditionalView                 (bool _isAdditionalView);
        VG_INLINE bool                      isAdditionalView                    () const;
        void                                clearShadowViews                    ();
        const ShadowView *                  findShadowView                      (const LightInstance * _light) const;
        core::string                        findShadowMapID                     (const LightInstance * _light) const;

        void                                setOrtho                            (bool _ortho);

        static core::float4x4               setPerspectiveProjectionRH          (float _fov, float _ar, float _near, float _far);
        static core::float4x4               setOrthoProjectionRH                (float _w, float _h, float _near, float _far);
        static core::float4x4               setOrthoProjectionOffCenterRH       (float _left, float _right, float _bottom, float _top, float _nearZ, float _farZ);
        static core::float4x4               lookAtRH                            (const core::float3 & _eye, const core::float3 & _target, const core::float3 & _zUp);

    private:
        void                                computeCameraFrustum                ();

    private:
        IViewport *                         m_viewport                          = nullptr;
        const core::IGameObject *           m_cameraGO                          = nullptr;
        PickingID                           m_pickingID                         = 0;
        const ICameraSettings *             m_cameraSettings                    = nullptr;
        gfx::ViewID                         m_viewID;
        ViewFlags                           m_flags                             = (ViewFlags)0;
        gfx::Texture *                      m_renderTarget                      = nullptr;   // use 'nullptr' for backbuffer
        core::uint2                         m_renderTargetSize                  = core::uint2(0, 0);
        core::uint2                         m_size                              = core::uint2(0, 0);
        core::int2                          m_offset                            = core::int2(0, 0);
        core::float4x4                      m_view                              = core::float4x4::identity();
        core::float4x4                      m_viewInv                           = core::float4x4::identity();
        core::float4x4                      m_proj                              = core::float4x4::identity();
        core::float4x4                      m_projInv                           = core::float4x4::identity();
        core::float4x4                      m_viewProj                          = core::float4x4::identity();
        core::IWorld *                      m_camWorld                          = nullptr;
        core::float2                        m_cameraNearFar;
        float                               m_cameraFovY;
        core::float2                        m_viewportOffset                    = core::float2(0,0);
        core::float2                        m_viewportScale                     = core::float2(1,1);

        core::vector<ViewCullingSplitJob *> m_cullingSplitJobs;
        core::vector<ViewCullingJobOutput>  m_cullingSplitJobsResults;

        ViewCullingFinalJob *               m_cullingJob                        = nullptr;
        ViewCullingJobOutput                m_cullingJobResult;

        ViewConstantsUpdatePass *           m_viewConstantsUpdatePass           = nullptr;
        core::uint2                         m_mouseOffset;
        PickingData                         m_rawPickingData;
        core::vector<PickingHit>            m_pickingHits;
        mutable core::AssertMutex           m_pickingHitsAssertMutex            = core::AssertMutex("PickingHits");
        gfx::TLAS *                         m_tlas                              = nullptr;
        TLASMode                            m_tlasMode                          = (TLASMode)-1;
        Frustum                             m_frustum;

        core::vector<ShadowView*>           m_shadowViews;
        UIRenderer *                        m_viewGUI                           = nullptr;
    };
}

#if VG_ENABLE_INLINE
#include "View.inl"
#endif