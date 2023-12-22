#include "ViewConstantsUpdatePass.h"
#include "shaders/system/view.hlsli"
#include "Shaders/system/toolmode.hlsl.h"

namespace vg::renderer
{
    gfx::Buffer * ViewConstantsUpdatePass::s_ViewConstantsBuffer = nullptr;

    //--------------------------------------------------------------------------------------
    ViewConstantsUpdatePass::ViewConstantsUpdatePass() :
        UpdatePass("ViewConstantsUpdatePass")
    {
        auto * device = Device::get();

        if (nullptr == s_ViewConstantsBuffer)
        {
            BufferDesc viewConstantsBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, sizeof(ViewConstants));
            s_ViewConstantsBuffer = device->createBuffer(viewConstantsBufferDesc, "ViewConstants", nullptr, ReservedSlot::ViewConstantsBufSrv);
        }
    }

    //--------------------------------------------------------------------------------------
    ViewConstantsUpdatePass::~ViewConstantsUpdatePass()
    {
        VG_SAFE_RELEASE(s_ViewConstantsBuffer);
    }

    //--------------------------------------------------------------------------------------
    void ViewConstantsUpdatePass::Setup(const gfx::RenderPassContext & _renderPassContext, float _dt)
    {
        if (_renderPassContext.m_view->IsToolmode())
        {
            FrameGraphBufferResourceDesc toolmodeRWBufferDesc;
            toolmodeRWBufferDesc.elementSize = sizeof(ToolmodeRWBufferData);
            toolmodeRWBufferDesc.elementCount = 1;
        
            const auto toolmodeRWBufferID = _renderPassContext.getFrameGraphID("ToolmodeRWBuffer");
            createRWBuffer(toolmodeRWBufferID, toolmodeRWBufferDesc);
            writeRWBuffer(toolmodeRWBufferID); // declare write because we only want to be able to access the RWBuffer to get its RWBufferHandle 
        }
    }

    //--------------------------------------------------------------------------------------
    void ViewConstantsUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        const auto options = RendererOptions::get();

        u16 toolmodeRWBufferID = -1;
        if (_renderPassContext.m_view->IsToolmode())
        {
            Buffer * toolmodeRWBuffer = getRWBuffer(_renderPassContext.getFrameGraphID("ToolmodeRWBuffer"));
            toolmodeRWBufferID = toolmodeRWBuffer->getRWBufferHandle();
            _cmdList->clearRWBuffer(toolmodeRWBuffer, 0x0);
        }

        View * view = (View *)_renderPassContext.m_view;

        ViewConstants * constants = (ViewConstants*)_cmdList->map(s_ViewConstantsBuffer).data;
        {
            constants->setScreenSize(view->getSize());
            constants->setMousePos(view->GetRelativeMousePos());
            constants->setDisplayMode(options->getDisplayMode());
            constants->setRayTracingMode(options->getRayTracingMode());
            constants->setPostProcessMode(options->getPostProcessMode());
            constants->setDisplayFlags(options->getDisplayFlags());
            constants->setToolmodeRWBufferID(toolmodeRWBufferID);
            constants->setView(view->getViewMatrix());
            constants->setViewInv(view->getViewInvMatrix());
            constants->setProj(view->getProjMatrix());
            constants->setProjInv(view->getProjInvMatrix());
            constants->setTLASHandle(view->getTLASHandle());
        }
        _cmdList->unmap(s_ViewConstantsBuffer, constants);
    }
}
