#include "InstanceDataUpdatePass.h"
#include "Shaders/system/instancedata.hlsli"

namespace vg::renderer
{
    static const uint s_InstanceDataBufferSize = sizeof(GPUInstanceData) * 16384;

    //--------------------------------------------------------------------------------------
    InstanceDataUpdatePass::InstanceDataUpdatePass() :
        UpdatePass("InstanceDataUpdatePass")
    {
        auto * device = Device::get();

        BufferDesc instanceDataBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, s_InstanceDataBufferSize);
        m_instanceDataBuffer = device->createBuffer(instanceDataBufferDesc, "InstanceData", nullptr, ReservedSlot::InstanceDataBufSrv);
    }

    //--------------------------------------------------------------------------------------
    InstanceDataUpdatePass::~InstanceDataUpdatePass()
    {
        VG_SAFE_RELEASE(m_instanceDataBuffer);
    }

    //--------------------------------------------------------------------------------------
    // Estimate returns a cost of 1 per instance to update
    //--------------------------------------------------------------------------------------
    core::u64 InstanceDataUpdatePass::GetCostEstimate(const RenderPassContext & _renderContext) const
    {
        auto renderer = Renderer::get();
        const auto * cullingJobOutput = renderer->getSharedCullingJobOutput();
        VG_ASSERT(nullptr != cullingJobOutput);
        const auto & instances = cullingJobOutput->m_instances;
        return cullingJobOutput->m_instances.size();
    }

    //--------------------------------------------------------------------------------------
    void InstanceDataUpdatePass::BeforeAll(const gfx::RenderPassContext & _renderContext)
    {
        VG_PROFILE_CPU("InstanceData");

        auto renderer = Renderer::get();
        const auto * cullingJobOutput = renderer->getSharedCullingJobOutput();
        VG_ASSERT(nullptr != cullingJobOutput);
        const auto & instances = cullingJobOutput->m_instances;

        uint offset = 0;

        for (uint i = 0; i < instances.size(); ++i)
        {
            GraphicInstance * instance = instances[i];

            // Save offset
            instance->setGPUInstanceDataOffset(offset);
            
            const uint instanceDataSize = instance->GetGPUInstanceDataSize();
            offset += instanceDataSize;

            // Clear atomic flag
            instance->removeAtomicFlags(GraphicInstance::AtomicFlags::InstanceList);
        }

        m_mapSize = offset;
    }

    //--------------------------------------------------------------------------------------
    void InstanceDataUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        auto renderer = Renderer::get();
        const auto * cullingJobOutput = renderer->getSharedCullingJobOutput();
        VG_ASSERT(nullptr != cullingJobOutput);

        const auto & instances = cullingJobOutput->m_instances;

        size_t mapSize = m_mapSize;

        if (mapSize > 0)
        {
            // Alloc temp write buffer for instance data + max 256 materials per instance
            const uint bufferMaxSize = sizeof(GPUInstanceData) + 256 * sizeof(GPUMaterialData);
            u8 * buffer = (u8 *)alignUp((uint_ptr)alloca(bufferMaxSize + GPU_INSTANCE_DATA_ALIGNMENT - 1), (uint_ptr)GPU_INSTANCE_DATA_ALIGNMENT);
            VG_ASSERT_IS_ALIGNED(buffer, GPU_INSTANCE_DATA_ALIGNMENT);

            uint offset = 0;
            const u8 * VG_RESTRICT data = (const u8 * VG_RESTRICT)_cmdList->map(m_instanceDataBuffer, mapSize).data;
            {
                VG_ASSERT_IS_ALIGNED(data, GPU_INSTANCE_DATA_ALIGNMENT);
                for (uint i = 0; i < instances.size(); ++i)
                {
                    const GraphicInstance * instance = instances[i];
                    VG_ASSERT(instance->getGPUInstanceDataOffset() == offset);
                    const uint instanceDataSize = instance->FillGPUInstanceData(buffer);
                    VG_ASSERT(instanceDataSize <= bufferMaxSize);
                    memcpy((u8 *)data + offset, buffer, instanceDataSize);
                    offset += instanceDataSize;
                }
            }
            _cmdList->unmap(m_instanceDataBuffer);
            VG_ASSERT(offset == mapSize);
        }
    }
}
