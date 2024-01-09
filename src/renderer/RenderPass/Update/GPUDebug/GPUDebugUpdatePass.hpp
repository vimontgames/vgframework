#include "GPUDebugUpdatePass.h"
#include "Shaders/system/debug.hlsli"

namespace vg::renderer
{
    static const uint s_GPUDebugBufferSize = GPUDEBUGBUFFER_SIZE;

    //--------------------------------------------------------------------------------------
    GPUDebugUpdatePass::GPUDebugUpdatePass() :
        UpdatePass("GPUDebugUpdatePass")
    {
        auto * device = Device::get();

        BufferDesc GPUDebugBufferDesc = BufferDesc(Usage::Default, BindFlags::UnorderedAccess, CPUAccessFlags::Write, BufferFlags::None, s_GPUDebugBufferSize);
        m_GPUDebugBuffer = device->createBuffer(GPUDebugBufferDesc, "GPUDebug", nullptr, ReservedSlot::GPUDebugBufRW);
    }

    //--------------------------------------------------------------------------------------
    GPUDebugUpdatePass::~GPUDebugUpdatePass()
    {
        VG_SAFE_RELEASE(m_GPUDebugBuffer);
        VG_SAFE_RELEASE(m_GPUDebugBufferStaging);
    }

    //--------------------------------------------------------------------------------------
    void GPUDebugUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList)
    {
        if (m_GPUDebugBufferStaging != nullptr)
        {
            u32 * data = (u32*)m_GPUDebugBufferStaging->getResource().map().data;
            {
                if (data[0] != 0)
                {
                    char temp[256];
                    uint index = 0;
                    u8 * p = (u8*) &data[1];
                    while (*p != 0)
                    {
                        temp[index] = *p;
                        ++index;
                        ++p;
                    }
                    temp[index] = 0;
                    VG_INFO("[GPU] %s", temp);
                }
            }
            m_GPUDebugBufferStaging->getResource().unmap();
        }
        else
        {
            // allocate staging copy
            BufferDesc stagingDesc = m_GPUDebugBuffer->getBufDesc();
            stagingDesc.resource.m_usage = Usage::Staging;
            stagingDesc.resource.m_bindFlags = BindFlags::None;
            m_GPUDebugBufferStaging = gfx::Device::get()->createBuffer(stagingDesc, "GPUDebugStaging");
        }

        // copy to staging
        _cmdList->copyBuffer(m_GPUDebugBufferStaging, m_GPUDebugBuffer, 0);

        // Clear
        _cmdList->clearRWBuffer(m_GPUDebugBuffer, 0x0);
    }
}
