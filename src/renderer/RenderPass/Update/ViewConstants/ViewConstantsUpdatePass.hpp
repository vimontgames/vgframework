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
    void ViewConstantsUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList)
    {
        const auto options = DisplayOptions::get();

        ViewConstants constants = {};
        constants.setScreenSize(_renderContext.m_view->GetSize());
        constants.setMousePos(_renderContext.m_view->GetRelativeMousePos());
        constants.setDisplayMode(options->getDisplayMode());
        constants.setDisplayFlags(options->getDisplayFlags());

        ViewConstants * data = (ViewConstants*)_cmdList->map(s_ViewConstantsBuffer);
        {
            memcpy(data, &constants, sizeof(constants));
        }
        _cmdList->unmap(s_ViewConstantsBuffer, data);
    }
}
