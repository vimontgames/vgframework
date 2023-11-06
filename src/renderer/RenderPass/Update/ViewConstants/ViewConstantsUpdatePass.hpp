#include "ViewConstantsUpdatePass.h"
#include "shaders/system/view.hlsli"

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
    void ViewConstantsUpdatePass::Setup(const gfx::RenderPassContext & _renderPassContext, double _dt)
    {
        //if (_renderPassContext.m_view->IsToolmode())
        //{
        //    FrameGraphBufferResourceDesc toolmodeRWBufferDesc;
        //    toolmodeRWBufferDesc.elementSize = 16;
        //    toolmodeRWBufferDesc.elementCount = 1;
        //
        //    const auto toolmodeRWBufferID = _renderPassContext.getFrameGraphID("ToolmodeRWBuffer");
        //    createRWBuffer(toolmodeRWBufferID, toolmodeRWBufferDesc);
        //    writeRWBuffer(toolmodeRWBufferID); // declare write because we only want to be able to access the RWBuffer to get its RWBufferHandle 
        //}
    }

    //--------------------------------------------------------------------------------------
    void ViewConstantsUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        const auto options = DisplayOptions::get();

        ViewConstants constants = {};
        constants.setScreenSize(_renderPassContext.m_view->GetSize());
        constants.setMousePos(_renderPassContext.m_view->GetRelativeMousePos());
        constants.setDisplayMode(options->getDisplayMode());
        constants.setDisplayFlags(options->getDisplayFlags());

        //u16 toolmodeRWBufferID = -1;
        //if (_renderPassContext.m_view->IsToolmode())
        //    toolmodeRWBufferID = getRWBuffer(_renderPassContext.getFrameGraphID("ToolmodeRWBuffer"))->getRWBufferHandle();
        // 
        //constants.setToolmodeRWBufferID(toolmodeRWBufferID);        

        ViewConstants * data = (ViewConstants*)_cmdList->map(s_ViewConstantsBuffer);
        {
            memcpy(data, &constants, sizeof(constants));
        }
        _cmdList->unmap(s_ViewConstantsBuffer, data);
    }
}
