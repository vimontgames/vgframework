#include "renderer/Precomp.h"
#include "View.h"
#include "core/GameObject/GameObject.h"
#include "core/IInput.h"
#include "gfx/ITexture.h"
#include "gfx/Resource/Buffer.h"
#include "renderer/Renderer.h"
#include "renderer/Job/Culling/ViewCullingJob.h"
#include "renderer/RenderPass/Update/ViewConstants/ViewConstantsUpdatePass.h"

#if !VG_ENABLE_INLINE
#include "View.inl"
#endif

// Blob
#include "Forward/ForwardView.hpp"

using namespace vg::core;
using namespace vg::gfx;


namespace vg::renderer
{
    
    //--------------------------------------------------------------------------------------
    View::View(const CreateViewParams & _params) : 
        IView(_params)
    {
        m_viewID.target = _params.target;

        m_size = _params.size;
        m_offset = _params.offset;

        m_viewInv = float4x4
        (
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 1.0f
        );

        SetUniverse(_params.universe);

        if (_params.dest)
        {
            VG_SAFE_INCREASE_REFCOUNT(_params.dest);
            m_renderTarget = (Texture*)_params.dest;
        }

        m_cullingJob = new ViewCullingJob("ViewCulling", this, &m_cullingJobResult);

        m_viewConstantsUpdatePass = new ViewConstantsUpdatePass();

        memset(&m_rawPickingData, 0x0, sizeof(m_rawPickingData));
    }

    //--------------------------------------------------------------------------------------
    View::~View()
    {
        VG_SAFE_RELEASE(m_renderTarget);
        VG_SAFE_RELEASE(m_cullingJob);
        VG_SAFE_RELEASE(m_viewConstantsUpdatePass);
    }

    //--------------------------------------------------------------------------------------
    // Toolmode is always enabled for editor views
    //--------------------------------------------------------------------------------------
    bool View::isToolmode() const
    {
        const auto options = DisplayOptions::get();
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
    // 
    // sx = sy/_ar
    // sy = 1.0f/tan(fov*0.5f)  
    // q = far / (near - far);
    //
    // sx   0    0     0
    //  0  sy    0     0
    //  0   0    q    -1
    //  0   0   zn*q   0
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
    void View::SetupCamera(const core::float4x4 & _viewInv, core::float2 _nearFar, float _fovY)
    {
        m_viewInv = _viewInv;
        m_cameraNearFar = _nearFar;
        m_cameraFovY = _fovY;

        const auto size = getSize();
        const float fovY = getCameraFovY();
        const float2 nearFar = getCameraNearFar();
        const float ar = float(size.x) / float(size.y);

        float4x4 proj = setPerspectiveProjectionRH(fovY, ar, nearFar.x, nearFar.y);
        m_viewProj = mul(getViewInvMatrix(), proj);
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
        return getViewInvMatrix();
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
    void View::SetUniverse(IUniverse* _universe)
    {
        if (_universe != m_cameraUniverse)
            m_cameraUniverse = _universe;
    }

    //--------------------------------------------------------------------------------------
    IUniverse* View::GetUniverse() const
    {
        return getUniverse();
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

            if (m_active)
                VG_INFO("[View] View \"%s\" is now active", getName().c_str());
            else
                VG_INFO("[View] View \"%s\" is now inactive", getName().c_str());
        }
    }
    
    //--------------------------------------------------------------------------------------
    bool View::IsActive() const
    {
        return m_active;
    }

    //--------------------------------------------------------------------------------------
    void View::SetMouseOffset(const core::uint2 & _mouseOffset)
    {
        m_mouseOffset = _mouseOffset;
    }

    //--------------------------------------------------------------------------------------
    core::uint2 View::GetRelativeMousePos() const
    {
        auto input = Kernel::getInput();
        uint2 mousePos = input->GetMousePos();

        const auto renderer = Renderer::get();
        if (renderer->IsFullscreen())
            return mousePos;
        else
            return uint2(mousePos.x - m_mouseOffset.x, mousePos.y - m_mouseOffset.y);
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
    void View::RegisterFrameGraph(const gfx::RenderPassContext & _rc, gfx::FrameGraph & _frameGraph)
    {
        _frameGraph.addUserPass(_rc, m_viewConstantsUpdatePass, "ViewConstantsUpdatePass");
    }

    //--------------------------------------------------------------------------------------
    void View::SetPickingData(const PickingData & _pickingData)
    {
        // Copy last picking results
        m_rawPickingData = _pickingData;

        // Save hits
        const uint counter = m_rawPickingData.m_counter.x;
        m_pickingHits.clear();
        for (uint i = 0; i < counter; ++i)
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
}
