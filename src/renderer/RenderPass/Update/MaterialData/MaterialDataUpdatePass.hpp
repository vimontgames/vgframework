#include "MaterialDataUpdatePass.h"
#include "Shaders/system/materialdata.hlsli"
#include "renderer\Model\Material\MaterialManager.h"

namespace vg::renderer
{
    static const uint s_MaterialDataBufferSize = 16 * 1024;

    //--------------------------------------------------------------------------------------
    MaterialDataUpdatePass::MaterialDataUpdatePass() :
        UpdatePass("MaterialDataUpdatePass")
    {
        auto * device = Device::get();

        BufferDesc materialDataBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, s_MaterialDataBufferSize);
        m_materialDataBuffer = device->createBuffer(materialDataBufferDesc, "MaterialData", nullptr, ReservedSlot::MaterialDataBufSrv);
    }

    //--------------------------------------------------------------------------------------
    MaterialDataUpdatePass::~MaterialDataUpdatePass()
    {
        VG_SAFE_RELEASE(m_materialDataBuffer);
    }

    //--------------------------------------------------------------------------------------
    void MaterialDataUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        auto renderer = Renderer::get();
        const auto * cullingJobOutput = renderer->getSharedCullingJobOutput();
        VG_ASSERT(nullptr != cullingJobOutput);
        
        const auto & instances = cullingJobOutput->m_instances;
        const auto * defaultMaterial = renderer->getDefaultMaterial();
        VG_ASSERT(defaultMaterial);

        MaterialManager * materialManager = MaterialManager::get();

        // What if a material is added to materialManager during this loop? 
        // We need a mutex or some kind of "safe copy" to avoid multi threading issues here.        
        const auto & materials = materialManager->getMaterialsSafeCopy();
        size_t mapSize = materials.size() * sizeof(GPUMaterialData);
        
        if (mapSize > 0)
        {
            uint offset = 0;
            const u8 * data = (const u8 *)_cmdList->map(m_materialDataBuffer, mapSize).data;
            {
                for (uint i = 0; i < materials.size(); ++i)
                {
                    GPUMaterialData * dst = (GPUMaterialData *)(data + offset);

                    const auto * material = materials[i];
                    if (nullptr == material)
                        material = defaultMaterial;
                        
                    material->FillGPUMaterialData(dst);
                    offset += sizeof(GPUMaterialData);
                }
            }
            _cmdList->unmap(m_materialDataBuffer);
        
            VG_ASSERT(offset == mapSize);
        }
    }
}
