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
    void MaterialDataUpdatePass::BeforeAll(const gfx::RenderPassContext & _renderContext)
    {
        MaterialManager * materialManager = MaterialManager::get();
        materialManager->getMaterialsSafeCopy(m_materials);
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
        size_t mapSize = m_materials.size() * sizeof(GPUMaterialData);      
        
        if (mapSize > 0)
        {
            // Alloc temp write buffer for GPUMaterialData
            const uint bufferMaxSize = sizeof(GPUMaterialData);
            GPUMaterialData * buffer = (GPUMaterialData *)alignUp((uint_ptr)alloca(bufferMaxSize + GPU_INSTANCE_DATA_ALIGNMENT - 1), (uint_ptr)GPU_INSTANCE_DATA_ALIGNMENT);
            VG_ASSERT_IS_ALIGNED(buffer, sizeof(uint4));

            uint offset = 0;
            const u8 * VG_RESTRICT data = (const u8 * VG_RESTRICT)_cmdList->map(m_materialDataBuffer, mapSize).data;
            {
                VG_ASSERT_IS_ALIGNED(data, sizeof(uint4));
                for (uint i = 0; i < m_materials.size(); ++i)
                {
                    const auto * material = m_materials[i];
                    if (nullptr == material)
                        material = defaultMaterial;
                        
                    material->FillGPUMaterialData(buffer);
                    memcpy((GPUMaterialData *)(data + offset), buffer, sizeof(GPUMaterialData));
                    offset += sizeof(GPUMaterialData);
                }
            }
            _cmdList->unmap(m_materialDataBuffer);
        
            VG_ASSERT(offset == mapSize);
        }
    }
}
