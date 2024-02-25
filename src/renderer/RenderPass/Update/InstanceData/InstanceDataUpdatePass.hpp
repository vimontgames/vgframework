#include "InstanceDataUpdatePass.h"
#include "Shaders/system/instancedata.hlsli"

// TODO : handle different material data & size?
//#include "renderer/Model/Material/DefaultMaterial/DefaultMaterialModel.h"

namespace vg::renderer
{
    static const uint s_InstanceDataBufferSize = 16 * 1024;

    //--------------------------------------------------------------------------------------
    InstanceDataUpdatePass::InstanceDataUpdatePass() :
        UpdatePass("InstanceDataUpdatePass")
    {
        auto * device = Device::get();

        BufferDesc instanceDataBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, s_InstanceDataBufferSize);
        m_InstanceDataConstantsBuffer = device->createBuffer(instanceDataBufferDesc, "InstanceData", nullptr, ReservedSlot::InstanceDataBufSrv);
    }

    //--------------------------------------------------------------------------------------
    InstanceDataUpdatePass::~InstanceDataUpdatePass()
    {
        VG_SAFE_RELEASE(m_InstanceDataConstantsBuffer);
    }

    //--------------------------------------------------------------------------------------
    void InstanceDataUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        auto renderer = Renderer::get();
        const auto & instances = renderer->getSharedCullingJobOutput()->m_instances;
        const auto * defaultMaterial = renderer->getDefaultMaterial();
        VG_ASSERT(defaultMaterial);

        size_t mapSize = 0;

        // Fill empty instance data and default material at the beginning of the Buffer
        mapSize += sizeof(GPUInstanceData) + 1 * sizeof(GPUMaterialData);

        for (uint i = 0; i < instances.size(); ++i)
        {
            GraphicInstance * instance = instances[i];
            const uint materialCount = (uint)instance->getMaterials().size();

            mapSize += sizeof(GPUInstanceData) + materialCount * sizeof(GPUMaterialData);

            // clear atomic flag after processing
            instance->removeAtomicFlags(GraphicInstance::AtomicFlags::Instance);
        }

        if (mapSize > 0)
        {
            uint offset = 0;
            const u8 * data = (const u8 *)_cmdList->map(m_InstanceDataConstantsBuffer, mapSize).data;
            {
                // Whenever an instance data or material data is missing default instance/material data can be used instead
                {
                    GPUInstanceData * defaultHeader = (GPUInstanceData *)(data + offset);
                    {
                        defaultHeader->setMaterialCount(1);
                        defaultHeader->setInstanceColor(float4(1,1,1,1));
                    }
                    offset += sizeof(GPUInstanceData);

                    GPUMaterialData * defaultMatData = (GPUMaterialData *)(data + offset);
                    {
                        defaultMaterial->FillGPUMaterialData(defaultMatData);
                    }
                    offset += sizeof(GPUMaterialData);
                }

                for (uint i = 0; i < instances.size(); ++i)
                {
                    GraphicInstance * instance = instances[i];
                    const auto & materials = instance->getMaterials();
                    const uint materialCount = (uint)materials.size();

                    instance->setGPUInstanceDataOffset(offset);

                    GPUInstanceData * instanceHeader = (GPUInstanceData *)(data + offset);
                    {
                        instanceHeader->setMaterialCount(materialCount);
                        instanceHeader->setInstanceColor(instance->getColor());
                    }
                    offset += sizeof(GPUInstanceData);
                    
                    for (uint m = 0; m < materialCount; ++m)
                    {
                        GPUMaterialData * matData = (GPUMaterialData *)(data + offset);
                        const MaterialModel * matModel = materials[m];
                        if (nullptr == matModel)
                            matModel = defaultMaterial;

                        if (matModel)
                            matModel->FillGPUMaterialData(matData);

                        offset += sizeof(GPUMaterialData);
                    }                    
                }
            }
            _cmdList->unmap(m_InstanceDataConstantsBuffer);

            VG_ASSERT(offset == mapSize);
        }
    }
}
