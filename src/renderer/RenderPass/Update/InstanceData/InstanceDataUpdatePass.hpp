#include "InstanceDataUpdatePass.h"
#include "Shaders/system/instancedata.hlsli"

namespace vg::renderer
{
    static const uint s_InstanceDataBufferSize = 16 * 1024;

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
    void InstanceDataUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        auto renderer = Renderer::get();
        const auto * cullingJobOutput = renderer->getSharedCullingJobOutput();
        VG_ASSERT(nullptr != cullingJobOutput);

        const auto & instances = cullingJobOutput->m_instances;
        const auto * defaultMaterial = renderer->getDefaultMaterial();
        VG_ASSERT(defaultMaterial);
        const auto defaultMaterialHandle = defaultMaterial->getMaterialDataGPUHandle();
        VG_ASSERT(defaultMaterialHandle == 0);

        size_t mapSize = 0;

        mapSize += sizeof(GPUInstanceData);

        for (uint i = 0; i < instances.size(); ++i)
        {
            GraphicInstance * instance = instances[i];
            const uint materialCount = (uint)instance->getMaterials().size();

            mapSize += sizeof(GPUInstanceData);

            // clear atomic flag after processing
            instance->removeAtomicFlags(GraphicInstance::AtomicFlags::Instance);
        }

        if (mapSize > 0)
        {
            uint offset = 0;
            const u8 * data = (const u8 *)_cmdList->map(m_instanceDataBuffer, mapSize).data;
            {
                // Whenever an instance data or material data is missing default instance/material data can be used instead
                {
                    GPUInstanceData * defaultInstanceData = (GPUInstanceData *)(data + offset);
                    {
                        defaultInstanceData->setMaterialCount(1);
                        defaultInstanceData->setInstanceColor(float4(1,1,1,1));
                        defaultInstanceData->setMaterialHandle(0, defaultMaterialHandle);
                    }
                    offset += sizeof(GPUInstanceData);
                }

                for (uint i = 0; i < instances.size(); ++i)
                {
                    GraphicInstance * instance = instances[i];
                    const auto & materials = instance->getMaterials();

                    VG_ASSERT(materials.size() < MAX_MATERIAL_PER_INSTANCE);
                    const uint materialCount = min((uint)materials.size(), (uint)MAX_MATERIAL_PER_INSTANCE);

                    instance->setGPUInstanceDataOffset(offset);

                    GPUInstanceData * instanceData = (GPUInstanceData *)(data + offset);
                    {
                        instanceData->setMaterialCount(materialCount);
                        instanceData->setInstanceColor(instance->getColor());

                        for (uint m = 0; m < materialCount; ++m)
                        {
                            const MaterialModel * mat = materials[m];
                            if (nullptr != mat)
                                instanceData->setMaterialHandle(m, mat->getMaterialDataGPUHandle());
                            else
                                instanceData->setMaterialHandle(m, defaultMaterialHandle);
                        }
                    }
                    offset += sizeof(GPUInstanceData);
                }
            }
            _cmdList->unmap(m_instanceDataBuffer);

            VG_ASSERT(offset == mapSize);
        }
    }
}
