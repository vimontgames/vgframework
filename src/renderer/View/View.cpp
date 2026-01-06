#include "renderer/Precomp.h"
#include "View.h"
#include "core/GameObject/GameObject.h"
#include "core/IInput.h"
#include "core/IWorld.h"
#include "core/Scheduler/Mutex.h"
#include "core/Scheduler/Scheduler.h"
#include "gfx/ITexture.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Raytracing/TLAS.h"
#include "renderer/Renderer.h"
#include "renderer/Job/Culling/ViewCullingJob.h"
#include "renderer/RenderPass/Update/ViewConstants/ViewConstantsUpdatePass.h"
#include "renderer/UI/UIRenderer.h"
#include "renderer/Picking/PickingManager.h"
#include "renderer/Renderer_consts.h"
#include "renderer/Camera/CameraSettings.h"
#include "renderer/Camera/CameraLens.h"
#include "renderer/Scene/SceneRenderData.h"

#if !VG_ENABLE_INLINE
#include "View.inl"
#endif

#include "Shaders/system/packing.hlsli"

#include "Lit/LitView.hpp"
#include "Shadow/ShadowView.hpp"
#include "Frustum.hpp"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    View::View(const CreateViewParams & _params)
    {
        m_viewport = _params.viewport;
        m_viewID.target = _params.target;

        SetRenderTargetSize(_params.size);

        SetWorld(_params.world);

        if (_params.dest)
        {
            VG_SAFE_INCREASE_REFCOUNT(_params.dest);
            m_renderTarget = (Texture *)_params.dest;
        }

        m_cullingJob = new ViewCullingFinalJob("ViewCulling", this);

        m_viewConstantsUpdatePass = new ViewConstantsUpdatePass();

        memset(&m_rawPickingData, 0x0, sizeof(m_rawPickingData));

        m_viewGUI = new UIRenderer(m_viewport, this);
    }

    //--------------------------------------------------------------------------------------
    View::~View()
    {
        VG_SAFE_DELETE(m_viewGUI);
        VG_SAFE_RELEASE(m_renderTarget);
        VG_SAFE_RELEASE(m_cullingJob);
        VG_SAFE_RELEASE(m_viewConstantsUpdatePass);
        VG_SAFE_RELEASE(m_tlas);

        for (uint j = 0; j < m_cullingSplitJobs.size(); ++j)
        {
            VG_SAFE_RELEASE(m_cullingSplitJobs[j]);
            m_cullingSplitJobsResults[j].clear();
        }
        m_cullingSplitJobs.clear();
        m_cullingSplitJobsResults.clear();
    }

    //--------------------------------------------------------------------------------------
    // Return at least one job or need to handle the case of 0 job sync
    //--------------------------------------------------------------------------------------
    core::vector<ViewCullingSplitJob *> View::getCullingSplitJobs()
    {
        const uint jobCount = Kernel::getScheduler()->GetWorkerThreadCount();

        if (m_cullingSplitJobs.size() != jobCount || m_cullingSplitJobsResults.size() != jobCount)
        {
            VG_PROFILE_CPU("Create jobs");

            for (uint j = 0; j < m_cullingSplitJobs.size(); ++j)
                VG_SAFE_RELEASE(m_cullingSplitJobs[j]);

            m_cullingSplitJobs.clear();
            m_cullingSplitJobsResults.clear();

            m_cullingSplitJobs.reserve(jobCount);
            m_cullingSplitJobsResults.reserve(jobCount);

            for (uint j = 0; j < jobCount; ++j)
            {
                ViewCullingJobOutput output;
                m_cullingSplitJobsResults.push_back(output);
                m_cullingSplitJobs.push_back(new ViewCullingSplitJob(fmt::sprintf("ViewCullingSplitJob #%u", j), (View *)this, &m_cullingSplitJobsResults[j], Renderer::get()->getSharedCullingJobOutput(), j));
            }
        }

        return m_cullingSplitJobs;
    }

    //--------------------------------------------------------------------------------------
    // Return 'true' if any job actually spawned, because it means we'll have to wait for sync
    //--------------------------------------------------------------------------------------
    bool View::startJobs()
    {
        VG_PROFILE_CPU("Start jobs");

        const auto renderer = Renderer::get();
        const auto options = RendererOptions::get();

        core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();

        if (RendererOptions::get()->isSplitCullingJobsEnabled())
        {
            JobSync cullingJobSync;

            // N jobs for culling scene parts
            const auto & cullingSplitJobs = getCullingSplitJobs();
            const auto jobCount = cullingSplitJobs.size();

            // Count registered visuals to split work evenly
            uint count = 0;
            const auto * world = getWorld();
            for (uint iSceneType = 0; iSceneType < enumCount<BaseSceneType>(); iSceneType++)
            {
                BaseSceneType sceneType = (BaseSceneType)iSceneType;
                if (sceneType != BaseSceneType::Scene)
                    continue;

                const uint sceneCount = world->GetSceneCount(sceneType);
                for (uint jScene = 0; jScene < sceneCount; ++jScene)
                {
                    const IBaseScene * scene = world->GetScene(jScene, sceneType);
                    SceneRenderData * sceneRenderData = (SceneRenderData *)scene->GetSceneRenderData();
                    VG_ASSERT(sceneRenderData);

                    count += sceneRenderData->getTotalGraphicInstanceCount();
                }
            }

            const uint maxGraphicInstancesPerJob = (uint)((count + jobCount - 1) / jobCount);
            uint offset = 0;
            uint remaining = count;
            for (uint i = 0; i < jobCount; ++i)
            {
                ViewCullingSplitJob * splitJob = cullingSplitJobs[i];
                core::uint rangeCount = min(maxGraphicInstancesPerJob, remaining);
                splitJob->setup(offset, rangeCount);
                jobScheduler->Start(splitJob, &cullingJobSync);

                offset += rangeCount;
                remaining -= rangeCount;
            }
            VG_ASSERT(remaining == 0);

            // The 'Final' job will only start after all split jobs are done 
            m_cullingJob->setup(&m_cullingJobResult, Renderer::get()->getSharedCullingJobOutput(), &m_cullingSplitJobsResults);
            jobScheduler->StartAfter(&cullingJobSync, getCullingFinalJob(), renderer->GetJobSync(RendererJobType::ViewCulling));
        }
        else
        {
            // Use a single job for each view's culling
            m_cullingJob->setup(&m_cullingJobResult, Renderer::get()->getSharedCullingJobOutput());
            jobScheduler->Start(getCullingFinalJob(), renderer->GetJobSync(RendererJobType::ViewCulling));
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    void View::addShadowView(ShadowView * _shadowView)
    {
        _shadowView->SetViewID(ViewID(ViewTarget::Shadow, (ViewIndex)m_shadowViews.size()));
        m_shadowViews.push_back(_shadowView);
    }

    //--------------------------------------------------------------------------------------
    void View::clearShadowViews()
    {
        m_shadowViews.clear();
    }

    //--------------------------------------------------------------------------------------
    // TODO : Use hashtable to speed up search?
    //--------------------------------------------------------------------------------------
    const ShadowView * View::findShadowView(const LightInstance * _light) const
    {
        for (uint i = 0; i < m_shadowViews.size(); ++i)
        {
            const auto * shadowView = m_shadowViews[i];
            if (_light == shadowView->getLight())
                return shadowView;
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    core::string View::findShadowMapID(const LightInstance * _light) const
    {
        if (const auto * shadowView = findShadowView(_light))
            return shadowView->getShadowMapName(this);

        return {};
    }

    //--------------------------------------------------------------------------------------
    // Toolmode is always enabled for editor views
    //--------------------------------------------------------------------------------------
    bool View::isToolmode() const
    {
        const auto options = RendererOptions::get();
        const auto target = getViewID().target;
        return  ViewTarget::Editor == target || (ViewTarget::Game == target && options->isToolModeEnabled());
    }

    //--------------------------------------------------------------------------------------
    void View::SetRenderTarget(ITexture * _renderTarget)
    {
        setRenderTarget((Texture*)_renderTarget);
    }

    //--------------------------------------------------------------------------------------
    ITexture * View::GetRenderTarget() const
    {
        return (ITexture*)getRenderTarget();
    }

    //--------------------------------------------------------------------------------------
    // Setup Right-Handed perspective projection matrix:
    //--------------------------------------------------------------------------------------
    float4x4 View::setPerspectiveProjectionRH(float _fov, float _ar, float _near, float _far)
    {
        const float sy = 1.0f / tan(_fov*0.5f);
        const float sx = sy / _ar;
        const float q = _far / (_near - _far);
        const float nq = _near * q;

        float4x4 mProj
        (
            sx, 0, 0,  0,
            0, sy, 0,  0,
            0, 0,  q, -1,
            0, 0, nq,  0
        );

        return mProj;
    }

    //--------------------------------------------------------------------------------------
    // Setup Right-Handed orthographic projection matrix:
    //--------------------------------------------------------------------------------------
    float4x4 View::setOrthoProjectionRH(float _w, float _h, float _near, float _far)
    {
        const float d = _near - _far;

        float4x4 mProj
        (
            2.0f/_w, 0,           0,   0,
                  0, 2.0f/_h,     0,   0,
                  0, 0,       1.0f/d,  0,
                  0, 0,      _near/d,  1
        );

        return mProj;
    }

    //--------------------------------------------------------------------------------------
    // Setup Right-Handed orthographic projection matrix:
    //--------------------------------------------------------------------------------------
    core::float4x4 View::setOrthoProjectionOffCenterRH(float _left, float _right, float _bottom, float _top, float _nearZ, float _farZ)
    {
        const float d = _nearZ - _farZ; // même convention que setOrthoProjectionRH
        const float w = _right - _left;
        const float h = _top - _bottom;

        core::float4x4 mProj
        (
            2.0f / w, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / h, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f / d, 0.0f,
            -(_right + _left) / w, -(_top + _bottom) / h, _nearZ / d, 1.0f
        );

        return mProj;

        return mProj;
    }

    //--------------------------------------------------------------------------------------
    // Right-Handed lookAt matrix:
    //--------------------------------------------------------------------------------------
    float4x4 View::lookAtRH(const core::float3 & _eye, const core::float3 & _target, const core::float3 & _up)
    {
        float3 zaxis = normalize(_eye - _target);       // Forward 
        float3 xaxis = normalize(cross(_up, zaxis));    // Right
        float3 yaxis = cross(zaxis, xaxis);             // Up

        float4x4 matrix
        (
            xaxis.x, yaxis.x, zaxis.x, 0.0f,
            xaxis.y, yaxis.y, zaxis.y, 0.0f,
            xaxis.z, yaxis.z, zaxis.z, 0.0f,
           -dot(xaxis, _eye), -dot(yaxis, _eye), -dot(zaxis, _eye), 1.0f
        );

        return matrix;
    }

    //--------------------------------------------------------------------------------------
    void View::SetupPerspectiveCamera(const core::float4x4 & _cameraWorldMatrix, core::float2 _nearFar, float _fovY, core::float2 _viewportOffset, core::float2 _viewportScale, const core::IGameObject * _cameraGO, PickingID _pickingID)
    {
        m_cameraGO = _cameraGO;
        m_pickingID = _pickingID;
        m_viewportOffset = _viewportOffset;
        m_viewportScale = _viewportScale;

        m_viewInv = _cameraWorldMatrix;
        m_view = inverse(_cameraWorldMatrix);
        m_cameraNearFar = _nearFar;

        m_cameraFovY = _fovY;

        const auto size = GetSize();
        const float ar = float(size.x) / float(size.y);

        m_proj = setPerspectiveProjectionRH(_fovY, ar, _nearFar.x, _nearFar.y);

        m_projInv = inverse(m_proj);
        m_viewProj = mul(m_view, m_proj);

        computeCameraFrustum();
    }

    //--------------------------------------------------------------------------------------
    void View::SetupOrthographicCamera(const core::float4x4 & _cameraWorldMatrix, core::uint2 _size, core::float2 _nearFar, const core::IGameObject * _cameraGO, PickingID _pickingID)
    {
        m_cameraGO = _cameraGO;
        m_pickingID = _pickingID;
        m_viewInv = _cameraWorldMatrix;
        m_view = inverse(_cameraWorldMatrix);
        m_cameraNearFar = _nearFar;

        m_proj = setOrthoProjectionRH((float)_size.x, (float)_size.y, _nearFar.x, _nearFar.y);
 
        m_projInv = inverse(m_proj);
        m_viewProj = mul(m_view, m_proj);

        computeCameraFrustum();
    }

    //--------------------------------------------------------------------------------------
    void View::SetupOrthographicCameraOffCenter(const core::float4x4 & _cameraWorldMatrix, float _left, float _right, float _bottom, float _top, float _nearZ, float _farZ, const core::IGameObject * _cameraGO, PickingID _pickingID)
    {
        m_cameraGO = _cameraGO;
        m_pickingID = _pickingID;
        m_viewInv = _cameraWorldMatrix;
        m_view = inverse(_cameraWorldMatrix);
        m_cameraNearFar = float2(_nearZ, _farZ);

        m_proj = setOrthoProjectionOffCenterRH(_left, _right, _bottom, _top, _nearZ, _farZ);

        m_projInv = inverse(m_proj);
        m_viewProj = mul(m_view, m_proj);

        computeCameraFrustum();
    }

    //--------------------------------------------------------------------------------------
    void View::SetupPhysicalCamera(const core::float4x4 & _cameraWorldMatrix, const ICameraSettings * _cameraSettings, core::float2 _viewportOffset, core::float2 _viewportScale, const core::IGameObject * _cameraGO, PickingID _pickingID)
    {
        m_cameraGO = _cameraGO;
        m_pickingID = _pickingID;
        m_cameraSettings = _cameraSettings;

        float fovX, fovY;

        // Aspect ratio based on the screen size
        const float2 screenSize = (float2)GetSize();
        const float screenAspectRatio = screenSize.x / screenSize.y;

        const float2 sensorSize = _cameraSettings->GetCameraLens()->GetSensorSize();
        const float sensorAspectRatio = sensorSize.x / sensorSize.y;

        const float focalLenght = _cameraSettings->GetFocalLength();

        const GateFitMode gateFitMode = _cameraSettings->GetGateFitMode();

        switch (gateFitMode)
        {
            case GateFitMode::Horizontal:
                fovX = 2.0f * std::atan(sensorSize.x / (2.0f * focalLenght));
                fovY = 2.0f * std::atan((sensorSize.x / screenAspectRatio) / (2.0f * focalLenght));
                break;

            case GateFitMode::Vertical:
                fovY = 2.0f * std::atan(sensorSize.y / (2.0f * focalLenght));
                fovX = 2.0f * std::atan((sensorSize.y * screenAspectRatio) / (2.0f * focalLenght));
                break;

            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(gateFitMode);
                break;
        }

        const float nearPlane = _cameraSettings->GetNear();
        const float farPlane = _cameraSettings->GetFar();

        m_viewportOffset = _viewportOffset;
        m_viewportScale = _viewportScale;

        m_viewInv = _cameraWorldMatrix;
        m_view = inverse(_cameraWorldMatrix);
        m_cameraNearFar = float2(nearPlane, farPlane);

        m_cameraFovY = fovY;

        m_proj = setPerspectiveProjectionRH(fovY, screenAspectRatio, nearPlane, farPlane);

        m_projInv = inverse(m_proj);
        m_viewProj = mul(m_view, m_proj);

        computeCameraFrustum();
    }

    //--------------------------------------------------------------------------------------
    void View::computeCameraFrustum()
    {
        const float4x4 viewProj = mul(m_view, m_proj);
        m_frustum.compute(viewProj);

        if (m_viewID.target == ViewTarget::Game )
        {
            const auto * world = getWorld();

            if (m_cameraGO->GetWorld() == world)
            {
                if (RendererOptions::get()->isDebugCameraFrustumEnabled())
                {
                    const u32 color = m_cameraGO ? packRGBA8(m_cameraGO->GetColor()) : 0x7F7F7F7F;
                    m_frustum.draw(world, color);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void View::SetFlags(ViewFlags _flagsToSet, ViewFlags _flagsToRemove)
    {
        setFlags(_flagsToSet, _flagsToRemove);
    }

    //--------------------------------------------------------------------------------------
    ViewFlags View::GetFlags() const
    {
        return getFlags();
    }

    //--------------------------------------------------------------------------------------
    const core::float4x4 & View::GetViewInvMatrix() const
    {
        return getViewInvMatrix();
    }
    //--------------------------------------------------------------------------------------
    const core::float4x4 & View::GetViewMatrix() const
    {
        return getViewMatrix();
    }

    //--------------------------------------------------------------------------------------
    const core::float4x4 & View::GetProjectionMatrix() const
    {
        return getProjMatrix();
    }

    //--------------------------------------------------------------------------------------
    core::float2 View::GetCameraNearFar() const
    {
        return getCameraNearFar();
    }

    //--------------------------------------------------------------------------------------
    float View::GetCameraFovY() const
    {
        return getCameraFovY();
    }

    //--------------------------------------------------------------------------------------
    void View::SetWorld(IWorld* _world)
    {
        if (_world != m_camWorld)
            m_camWorld = _world;
    }

    //--------------------------------------------------------------------------------------
    IWorld* View::GetWorld() const
    {
        return getWorld();
    }

    //--------------------------------------------------------------------------------------
    void View::SetRenderTargetSize(core::uint2 _size)
    {
        m_renderTargetSize = _size;
        m_size = max(uint2(1, 1), (uint2)((float2)m_renderTargetSize * getViewportScale()));   // TODO: handle view being resize to 0 during rendering
        m_offset = (int2)((float2)m_renderTargetSize * getViewportOffset());
    }

    //--------------------------------------------------------------------------------------
    core::uint2 View::GetRenderTargetSize() const
    {
        return m_renderTargetSize;
    }

    //--------------------------------------------------------------------------------------
    core::uint2 View::GetSize() const
    {
        return m_size;
    }

    //--------------------------------------------------------------------------------------
    core::int2 View::GetOffset() const
    {
        return m_offset;
    }

    //--------------------------------------------------------------------------------------
    core::float2 View::GetViewportOffset() const
    {
        return getViewportOffset();
    }

    //--------------------------------------------------------------------------------------
    core::float2 View::GetViewportScale() const
    {
        return getViewportScale();
    }

    //--------------------------------------------------------------------------------------
    IViewport * View::GetViewport() const 
    {
        return m_viewport;
    }

    //--------------------------------------------------------------------------------------
    void View::SetViewID(ViewID _viewID)
    {
        setViewID(_viewID);
    }

    //--------------------------------------------------------------------------------------
    ViewID View::GetViewID() const
    {
        return getViewID();
    }

    //--------------------------------------------------------------------------------------
    void View::SetFocused(bool _active)
    {
        if (testFlag(ViewFlags::Focus) != _active)
            setFlag(ViewFlags::Focus, _active);
    }
    
    //--------------------------------------------------------------------------------------
    bool View::IsFocused() const
    {
        return testFlag(ViewFlags::Focus);
    }

    //--------------------------------------------------------------------------------------
    void View::SetVisible(bool _visible)
    {
        if (testFlag(ViewFlags::Visible) != _visible)
            setFlag(ViewFlags::Visible, _visible);
    }

    //--------------------------------------------------------------------------------------
    bool View::IsVisible() const
    {
        return testFlag(ViewFlags::Visible) && all(GetRenderTargetSize() > 1);
    }

    //--------------------------------------------------------------------------------------
    void View::SetRender(bool _render)
    {
        if (testFlag(ViewFlags::Render) != _render)
            setFlag(ViewFlags::Render, _render);
    }

    //--------------------------------------------------------------------------------------
    bool View::IsRender() const
    {
        return IsVisible() && testFlag(ViewFlags::Render);// && nullptr != GetCameraSettings();
    }     

    //--------------------------------------------------------------------------------------
    void View::SetMouseOffset(const core::uint2 & _mouseOffset)
    {
        m_mouseOffset = _mouseOffset;
    }

    //--------------------------------------------------------------------------------------
    core::int2 View::GetRelativeMousePos() const
    {
        auto input = Kernel::getInput();
        uint2 mousePos = input->GetMousePos();
        int2 offset = (int2)((float2)GetRenderTargetSize() * GetViewportOffset());  

        const auto renderer = Renderer::get();
        if (renderer->IsFullscreen())
            return ((int2)mousePos)- offset;
        else
            return (int2((int)mousePos.x - (int)m_mouseOffset.x, (int)mousePos.y - (int)m_mouseOffset.y))- offset;
    }

    //--------------------------------------------------------------------------------------
    bool View::IsMouseOverView() const
    {
        const auto mousePos = GetRelativeMousePos();
        const int2 viewSize = (int2)GetSize();
        return all(mousePos.xy >= 0) && all(mousePos.xy < viewSize.xy);
    }

    //--------------------------------------------------------------------------------------
    const core::string View::GetFrameGraphID(const core::string & _name) const
    {
        return RenderPassContext::MakeFrameGraphID(_name, m_viewID);
    }

    //--------------------------------------------------------------------------------------
    bool View::IsToolmode() const 
    {
        return isToolmode();
    }

    //--------------------------------------------------------------------------------------
    bool View::IsOrtho() const 
    {
        return testFlag(ViewFlags::Ortho);
    }

    //--------------------------------------------------------------------------------------
    void View::setOrtho(bool _ortho)
    {
        if (testFlag(ViewFlags::Ortho) != _ortho)
            setFlag(ViewFlags::Ortho, _ortho);
    }

    //--------------------------------------------------------------------------------------
    bool View::IsUsingRayTracing() const
    {
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool View::IsLit() const
    {
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool View::IsComputePostProcessNeeded() const
    {
        const auto options = RendererOptions::get();
        return options->isPostProcessEnabled() || (isToolmode() && IsUsingRayTracing() && options->anyRayTracingDebugDisplay());
    }

    //--------------------------------------------------------------------------------------
    bool View::IsOutlinePassNeeded() const
    {
        const auto options = RendererOptions::get();
        return IsComputePostProcessNeeded() && isToolmode();
    }

    //--------------------------------------------------------------------------------------
    void View::setTLAS(gfx::TLAS * _tlas, TLASMode _mode)
    {
        VG_ASSERT(!_tlas || _mode != (TLASMode)-1, "You must specify TLAS mode when assigning TLAS to view");
        VG_ASSERT(!m_tlas || !_tlas || _mode == m_tlasMode);

        if (_tlas != m_tlas)
        {
            VG_SAFE_RELEASE_ASYNC(m_tlas);
            m_tlas = _tlas;
            m_tlasMode = _mode;
            VG_SAFE_INCREASE_REFCOUNT(_tlas);
        }
    }

    //--------------------------------------------------------------------------------------
    TLAS * View::getTLAS() const
    {
        return m_tlas;
    }

    //--------------------------------------------------------------------------------------
    TLASMode View::getTLASMode() const
    {
        return m_tlasMode;
    }

    //--------------------------------------------------------------------------------------
    BindlessTLASHandle View::getTLASHandle() const
    {
        TLAS * tlas = getTLAS();
        return tlas ? tlas->getHandle() : BindlessTLASHandle(BINDLESS_TLAS_INVALID);
    }

    //--------------------------------------------------------------------------------------
    void View::RegisterFrameGraph(const RenderPassContext & _rc, FrameGraph & _frameGraph)
    {
        _frameGraph.addUserPass(_rc, m_viewConstantsUpdatePass, "View Constants Update");
    }

    //--------------------------------------------------------------------------------------
    void View::SetPickingData(const PickingData & _pickingData)
    {
        // Copy last picking results
        m_rawPickingData = _pickingData;

        lock_guard<AssertMutex> _(m_pickingHitsAssertMutex);

        // Save hits
        const uint counter = m_rawPickingData.m_counter.x;
        m_pickingHits.clear();

        // Atomic counter is incremented for every possible hit, do not overread
        for (uint i = 0; i < min(counter, (uint)PICKING_MAX_HITS); ++i)
            m_pickingHits.push_back(m_rawPickingData.m_hits[i]);

        // Sort hits by depth stored in pos.w
        sort(m_pickingHits.begin(), m_pickingHits.end(), [](PickingHit & a, PickingHit & b)
        {
            #if VG_GFX_REVERSE_DEPTH
            return (a.m_pos.w > b.m_pos.w);
            #else
            return (a.m_pos.w < b.m_pos.w);
            #endif
        }
        );

        if (counter > PICKING_MAX_HITS)
        {
            VG_ASSERT(counter <= PICKING_MAX_HITS, "Picking hit %u pixels but buffer limit is %u. Please increase PICKING_MAX_HITS fix this error.", counter, PICKING_MAX_HITS);

            const PickingManager * pickingManager = PickingManager::get();
            for (uint i = 0; i < m_pickingHits.size(); ++i)
            {
                PickingHitInfo info;
                if (pickingManager->decodePickingHit(m_pickingHits[i], info))
                    VG_DEBUGPRINT("Hit %u: %s\n", i, info.m_gameObject->GetName().c_str());
            }
        }
     }

    //--------------------------------------------------------------------------------------
    void View::AddPickingHit(const PickingHit & _hit)
    {
        lock_guard<AssertMutex> _(m_pickingHitsAssertMutex);

        if (isToolmode())
            m_pickingHits.insert(m_pickingHits.begin(), _hit);
    }

    //--------------------------------------------------------------------------------------
    const PickingHit & View::GetPickingClosestHit() const
    {
        lock_guard<AssertMutex> _(m_pickingHitsAssertMutex);

        VG_ASSERT(GetPickingHitCount() > 0);
        return m_pickingHits[0];
    }

    //--------------------------------------------------------------------------------------
    const PickingHit & View::GetPickingHit(uint _index) const
    {
        lock_guard<AssertMutex> _(m_pickingHitsAssertMutex);

        return m_pickingHits[_index];
    }

    //--------------------------------------------------------------------------------------
    uint View::GetPickingHitCount() const
    {
        lock_guard<AssertMutex> _(m_pickingHitsAssertMutex);

        VG_ASSERT(m_pickingHits.size() <= PICKING_MAX_HITS);
        return (uint)m_pickingHits.size();
    }

    //--------------------------------------------------------------------------------------
    uint View::GetPickingRequestedHitCount() const
    {
        return (uint)m_rawPickingData.m_counter.x;
    }

    //--------------------------------------------------------------------------------------
    const ViewCullingJobOutput & View::getCullingJobResult() const
    {
        return m_cullingJobResult;
    }

    //--------------------------------------------------------------------------------------
    ViewCullingStats View::GetViewCullingStats() const
    {
        ViewCullingStats stats;

        stats.opaque        = (uint)m_cullingJobResult.get(GraphicInstanceListType::Opaque).m_instances.size();
        stats.alphatest     = (uint)m_cullingJobResult.get(GraphicInstanceListType::AlphaTest).m_instances.size();
        stats.transparent   = (uint)m_cullingJobResult.get(GraphicInstanceListType::Transparent).m_instances.size();
        stats.particleSystem = (uint)m_cullingJobResult.get(GraphicInstanceListType::Particle).m_instances.size();
        stats.decal         = (uint)m_cullingJobResult.get(GraphicInstanceListType::Decal).m_instances.size();
        stats.outline       = (uint)m_cullingJobResult.get(GraphicInstanceListType::Outline).m_instances.size();

        stats.directional   = (uint)m_cullingJobResult.get(LightType::Directional).m_instances.size();
        stats.omni          = (uint)m_cullingJobResult.get(LightType::Omni).m_instances.size();
        stats.spot          = (uint)m_cullingJobResult.get(LightType::Spot).m_instances.size();

        return stats;
    }

    //--------------------------------------------------------------------------------------
    core::vector<FrameGraphResourceID> View::getShadowMaps() const
    {
        const auto & shadowViews = getShadowViews();
        core::vector<FrameGraphResourceID> shadowMaps;
        shadowMaps.reserve(shadowViews.size());

        // Read shadow maps
        for (uint i = 0; i < shadowViews.size(); ++i)
        {
            ShadowView * shadowView = shadowViews[i];

            if (shadowView->isUsingShadowMap())
            {
                const auto shadowMapID = shadowView->getShadowMapName(this);
                shadowMaps.push_back(shadowMapID);
            }
        }

        return shadowMaps;
    }

    //--------------------------------------------------------------------------------------
    IUIRenderer * View::GetUIRenderer() const
    {
        return VG_SAFE_STATIC_CAST(IUIRenderer, m_viewGUI);
    }

    //--------------------------------------------------------------------------------------
    const ICameraSettings * View::GetCameraSettings() const
    {
        return m_cameraSettings;
    }
}
