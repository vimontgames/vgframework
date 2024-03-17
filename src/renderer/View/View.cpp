#include "renderer/Precomp.h"
#include "View.h"
#include "core/GameObject/GameObject.h"
#include "core/IInput.h"
#include "gfx/ITexture.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Raytracing/TLAS.h"
#include "renderer/Renderer.h"
#include "renderer/Job/Culling/ViewCullingJob.h"
#include "renderer/RenderPass/Update/ViewConstants/ViewConstantsUpdatePass.h"
#include "renderer/View/ViewGUI.h"

#if !VG_ENABLE_INLINE
#include "View.inl"
#endif

#include "Lit/LitView.hpp"
#include "Shadow/ShadowView.hpp"
#include "Frustum.hpp"
#include "ViewGUI.hpp"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    View::View(const CreateViewParams & _params)
    {
        m_viewID.target = _params.target;

        m_size = _params.size;
        m_offset = _params.offset;

        SetWorld(_params.world);

        if (_params.dest)
        {
            VG_SAFE_INCREASE_REFCOUNT(_params.dest);
            m_renderTarget = (Texture *)_params.dest;
        }

        m_cullingJob = new ViewCullingJob("ViewCulling", this, &m_cullingJobResult, Renderer::get()->getSharedCullingJobOutput());

        m_viewConstantsUpdatePass = new ViewConstantsUpdatePass();

        memset(&m_rawPickingData, 0x0, sizeof(m_rawPickingData));

        if (m_viewID.target == ViewTarget::Game)
            m_viewGUI = new ViewGUI();
    }

    //--------------------------------------------------------------------------------------
    View::~View()
    {
        VG_SAFE_DELETE(m_viewGUI);
        VG_SAFE_RELEASE(m_renderTarget);
        VG_SAFE_RELEASE(m_cullingJob);
        VG_SAFE_RELEASE(m_viewConstantsUpdatePass);
        VG_SAFE_RELEASE(m_tlas);
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
        return getViewID().target == gfx::ViewTarget::Editor || options->isToolModeEnabled();
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
                  0, 0,       1.0f/d, 0,
                  0, 0,      _near/d,  1
        );

        return mProj;
    }

    //--------------------------------------------------------------------------------------
    void View::SetupPerspectiveCamera(const core::float4x4 & _cameraWorldMatrix, core::float2 _nearFar, float _fovY)
    {
        m_viewInv = _cameraWorldMatrix;
        m_view = inverse(_cameraWorldMatrix);
        m_cameraNearFar = _nearFar;

        m_cameraFovY = _fovY;

        const auto size = getSize();
        const float ar = float(size.x) / float(size.y);

        m_proj = setPerspectiveProjectionRH(_fovY, ar, _nearFar.x, _nearFar.y);

        m_projInv = inverse(m_proj);
        m_viewProj = mul(m_view, m_proj);

        computeCameraFrustum();
    }

    //--------------------------------------------------------------------------------------
    void View::SetupOrthographicCamera(const core::float4x4 & _cameraWorldMatrix, core::uint2 _size, core::float2 _nearFar)
    {
        m_viewInv = _cameraWorldMatrix;
        m_view = inverse(_cameraWorldMatrix);
        m_cameraNearFar = _nearFar;

        m_proj = setOrthoProjectionRH((float)_size.x, (float)_size.y, _nearFar.x, _nearFar.y);

        m_projInv = inverse(m_proj);
        m_viewProj = mul(m_view, m_proj);

        computeCameraFrustum();
    }

    //--------------------------------------------------------------------------------------
    void View::computeCameraFrustum()
    {
        const float4x4 viewProj = mul(m_view, m_proj);

        m_frustum.planes[asInteger(FrustumPlane::Left)].x = viewProj._14 + viewProj._11;
        m_frustum.planes[asInteger(FrustumPlane::Left)].y = viewProj._24 + viewProj._21;
        m_frustum.planes[asInteger(FrustumPlane::Left)].z = viewProj._34 + viewProj._31;
        m_frustum.planes[asInteger(FrustumPlane::Left)].w = viewProj._44 + viewProj._41;

        m_frustum.planes[asInteger(FrustumPlane::Right)].x = viewProj._14 - viewProj._11;
        m_frustum.planes[asInteger(FrustumPlane::Right)].y = viewProj._24 - viewProj._21;
        m_frustum.planes[asInteger(FrustumPlane::Right)].z = viewProj._34 - viewProj._31;
        m_frustum.planes[asInteger(FrustumPlane::Right)].w = viewProj._44 - viewProj._41;

        m_frustum.planes[asInteger(FrustumPlane::Top)].x = viewProj._14 - viewProj._12;
        m_frustum.planes[asInteger(FrustumPlane::Top)].y = viewProj._24 - viewProj._22;
        m_frustum.planes[asInteger(FrustumPlane::Top)].z = viewProj._34 - viewProj._32;
        m_frustum.planes[asInteger(FrustumPlane::Top)].w = viewProj._44 - viewProj._42;

        m_frustum.planes[asInteger(FrustumPlane::Bottom)].x = viewProj._14 + viewProj._12;
        m_frustum.planes[asInteger(FrustumPlane::Bottom)].y = viewProj._24 + viewProj._22;
        m_frustum.planes[asInteger(FrustumPlane::Bottom)].z = viewProj._34 + viewProj._32;
        m_frustum.planes[asInteger(FrustumPlane::Bottom)].w = viewProj._44 + viewProj._42;

        m_frustum.planes[asInteger(FrustumPlane::Near)].x = viewProj._13;
        m_frustum.planes[asInteger(FrustumPlane::Near)].y = viewProj._23;
        m_frustum.planes[asInteger(FrustumPlane::Near)].z = viewProj._33;
        m_frustum.planes[asInteger(FrustumPlane::Near)].w = viewProj._43;

        m_frustum.planes[asInteger(FrustumPlane::Far)].x = viewProj._14 - viewProj._13;
        m_frustum.planes[asInteger(FrustumPlane::Far)].y = viewProj._24 - viewProj._23;
        m_frustum.planes[asInteger(FrustumPlane::Far)].z = viewProj._34 - viewProj._33;
        m_frustum.planes[asInteger(FrustumPlane::Far)].w = viewProj._44 - viewProj._43;

        for (uint i = 0; i < countof(m_frustum.planes); ++i)
        {
            float normXYZ = length(m_frustum.planes[i].xyz);
            m_frustum.planes[i] /= (float4)normXYZ;
        }
    }

    //--------------------------------------------------------------------------------------
    void View::SetFlags(Flags _flagsToSet, Flags _flagsToRemove)
    {
        setFlags(_flagsToSet, _flagsToRemove);
    }

    //--------------------------------------------------------------------------------------
    View::Flags View::GetFlags() const
    {
        return getFlags();
    }

    //--------------------------------------------------------------------------------------
    void View::setFlags(Flags _flagsToSet, Flags _flagsToRemove)
    {
        m_flags = (Flags)((std::underlying_type<Flags>::type(m_flags) & ~std::underlying_type<Flags>::type(_flagsToRemove)) | (std::underlying_type<Flags>::type(_flagsToSet)));
    }

    //--------------------------------------------------------------------------------------
    View::Flags View::getFlags() const
    {
        return m_flags;
    }

    //--------------------------------------------------------------------------------------
    const core::float4x4 & View::GetViewInvMatrix() const
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
    void View::SetSize(core::uint2 _size)
    {
        setSize(_size);
    }

    //--------------------------------------------------------------------------------------
    core::uint2 View::GetSize() const
    {
        return getSize();
    }

    //--------------------------------------------------------------------------------------
    void View::SetOffset(core::int2 _offset)
    {
        setOffset(_offset);
    }

    //--------------------------------------------------------------------------------------
    core::int2 View::GetOffset() const
    {
        return getOffset();
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
    void View::SetActive(bool _active)
    {
        if (m_active != _active)
        {
            m_active = _active;

            //if (m_active)
            //    VG_INFO("[View] View \"%s\" is now active", getName().c_str());
            //else
            //    VG_INFO("[View] View \"%s\" is no more active", getName().c_str());
        }
    }
    
    //--------------------------------------------------------------------------------------
    bool View::IsActive() const
    {
        return m_active;
    }

    //--------------------------------------------------------------------------------------
    void View::SetVisible(bool _visible)
    {
        if (m_visible != _visible)
        {
            m_visible = _visible;

            //if (m_visible)
            //    VG_INFO("[View] View \"%s\" is now visible", getName().c_str());
            //else
            //    VG_INFO("[View] View \"%s\" is no more visible", getName().c_str());
        }
    }

    //--------------------------------------------------------------------------------------
    bool View::IsVisible() const
    {
        return m_visible;
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

        const auto renderer = Renderer::get();
        if (renderer->IsFullscreen())
            return (int2)mousePos;
        else
            return int2((int)mousePos.x - (int)m_mouseOffset.x, (int)mousePos.y - (int)m_mouseOffset.y);
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
        return m_ortho;
    }

    //--------------------------------------------------------------------------------------
    void View::setOrtho(bool _ortho)
    {
        m_ortho = _ortho;
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
    void View::setTLAS(gfx::TLAS * _tlas)
    {
        if (_tlas != m_tlas)
        {
            VG_SAFE_RELEASE_ASYNC(m_tlas)
            m_tlas = _tlas;
        }
    }

    //--------------------------------------------------------------------------------------
    gfx::TLAS * View::getTLAS() const
    {
        return m_tlas;
    }

    //--------------------------------------------------------------------------------------
    gfx::BindlessTLASHandle View::getTLASHandle() const
    {
        TLAS * tlas = getTLAS();
        return tlas ? tlas->getHandle() : gfx::BindlessTLASHandle(BINDLESS_TLAS_INVALID);
    }

    //--------------------------------------------------------------------------------------
    void View::RegisterFrameGraph(const gfx::RenderPassContext & _rc, gfx::FrameGraph & _frameGraph)
    {
        _frameGraph.addUserPass(_rc, m_viewConstantsUpdatePass, "View Constants Update");
    }

    //--------------------------------------------------------------------------------------
    void View::SetPickingData(const PickingData & _pickingData)
    {
        // Copy last picking results
        m_rawPickingData = _pickingData;

        // Save hits
        const uint counter = m_rawPickingData.m_counter.x;
        m_pickingHits.clear();

        // Atomic counter is incremented for every possible hit, do not overread
        for (uint i = 0; i < min(counter, (uint)PICKING_MAX_HITS); ++i)
            m_pickingHits.push_back(m_rawPickingData.m_hits[i]);

        // Sort hits by depth stored in pos.w
        sort(m_pickingHits.begin(), m_pickingHits.end(), [](PickingHit & a, PickingHit & b)
        {
            if (a.m_pos.w < b.m_pos.w)
                return true;
            else
                return false;
        }
        );
     }

    //--------------------------------------------------------------------------------------
    const PickingHit & View::GetPickingClosestHit() const
    {
        VG_ASSERT(GetPickingHitCount() > 0);
        return m_pickingHits[0];
    }

    //--------------------------------------------------------------------------------------
    const PickingHit & View::GetPickingHit(uint _index) const
    {
        return m_pickingHits[_index];
    }

    //--------------------------------------------------------------------------------------
    uint View::GetPickingHitCount() const
    {
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
    gfx::ViewCullingStats View::GetViewCullingStats() const
    {
        ViewCullingStats stats;

        stats.opaque        = (uint)m_cullingJobResult.get(GraphicInstanceListType::Opaque).m_instances.size();
        stats.transparent   = (uint)m_cullingJobResult.get(GraphicInstanceListType::Transparent).m_instances.size();

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
    gfx::IViewGUI * View::GetViewGUI() const
    {
        return VG_SAFE_STATIC_CAST(gfx::IViewGUI, m_viewGUI);
    }
}
