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
    core::u64 InstanceDataUpdatePass::GetCostEstimate() const
    {
        auto renderer = Renderer::get();
        const auto * cullingJobOutput = renderer->getSharedCullingJobOutput();
        VG_ASSERT(nullptr != cullingJobOutput);
        const auto & instances = cullingJobOutput->m_instances;
        return cullingJobOutput->m_instances.size();
    }

    //--------------------------------------------------------------------------------------
    void InstanceDataUpdatePass::Prepare(const gfx::RenderPassContext & _renderContext)
    {
        VG_PROFILE_CPU("InstanceData");

        auto renderer = Renderer::get();
        const auto * cullingJobOutput = renderer->getSharedCullingJobOutput();
        VG_ASSERT(nullptr != cullingJobOutput);

        const auto & instances = cullingJobOutput->m_instances;
        const auto * defaultMaterial = renderer->getDefaultMaterial();
        VG_ASSERT(defaultMaterial);
        const auto defaultMaterialIndex = defaultMaterial->getGPUMaterialDataIndex();
        VG_ASSERT(defaultMaterialIndex == 0);

        uint offset = 0;

        for (uint i = 0; i < instances.size(); ++i)
        {
            GraphicInstance * instance = instances[i];
            const uint batchCount = instance->GetBatchCount();

            // Save offset
            instance->setGPUInstanceDataOffset(offset);
            offset += sizeof(GPUInstanceData) + batchCount * sizeof(GPUBatchData);

            // Clear atomic flag
            instance->removeAtomicFlags(GraphicInstance::AtomicFlags::Instance);
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
        const auto * defaultMaterial = renderer->getDefaultMaterial();
        VG_ASSERT(defaultMaterial);
        const auto defaultMaterialIndex = defaultMaterial->getGPUMaterialDataIndex();
        VG_ASSERT(defaultMaterialIndex == 0);

        size_t mapSize = m_mapSize;

        VG_ASSERT_IS_ALIGNED(sizeof(GPUInstanceData), GPU_INSTANCE_DATA_ALIGNMENT);

        if (mapSize > 0)
        {
            uint offset = 0;
            const u8 * VG_RESTRICT data = (const u8 * VG_RESTRICT)_cmdList->map(m_instanceDataBuffer, mapSize).data;
            {
                for (uint i = 0; i < instances.size(); ++i)
                {
                    const GraphicInstance * instance = instances[i];
                    VG_ASSERT(instance->getGPUInstanceDataOffset() == offset);

                    #if 1

                    instance->FillGPUInstanceData(data, offset);

                    #else

                    const auto & materials = instance->getMaterials();
                    const uint batchCount = instance->GetBatchCount();
                    const uint materialCount = (uint)materials.size();

                    GPUInstanceData * instanceData = (GPUInstanceData *)(data + offset);
                    {
                        VertexFormat vertexFormat;
                        if (instance->GetVertexFormat(vertexFormat))
                            instanceData->setVertexFormat(vertexFormat);
                        else
                            instanceData->setVertexFormat((VertexFormat)-1);

                        instanceData->setMaterialCount(materialCount);
                        instanceData->setInstanceColor(instance->getColor());

                        BindlessBufferHandle ib;
                        uint ibOffset, indexSize;

                        if (instance->GetIndexBuffer(ib, ibOffset, indexSize))
                            instanceData->setIndexBuffer(ib, indexSize, ibOffset);
                        else
                            instanceData->setIndexBuffer(-1);

                        BindlessBufferHandle vb;
                        uint vbOffset;
                        if (instance->GetVertexBuffer(vb, vbOffset))
                            instanceData->setVertexBuffer(vb, vbOffset);
                        else
                            instanceData->setVertexBuffer(-1);

                        for (uint b = 0; b < batchCount; ++b)
                        {
                            GPUBatchData * batchData = (GPUBatchData *)(data + offset + sizeof(GPUInstanceData) + b * sizeof(GPUBatchData));

                            if (b < materialCount)
                            {
                                const MaterialModel * mat = materials[b];
                                if (nullptr != mat)
                                {
                                    batchData->setMaterialIndex(mat->getGPUMaterialDataIndex());
                                    batchData->setStartIndex(instance->GetBatchOffset(b));
                                    continue;
                                }
                            }
                            
                            batchData->setMaterialIndex(defaultMaterialIndex);
                        }
                    }

                    offset += sizeof(GPUInstanceData) + batchCount * sizeof(GPUBatchData);

                    #endif                    
                }
            }
            _cmdList->unmap(m_instanceDataBuffer);
            VG_ASSERT(offset == mapSize);
        }
    }
}
