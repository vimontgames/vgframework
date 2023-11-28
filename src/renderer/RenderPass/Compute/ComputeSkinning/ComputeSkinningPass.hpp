#include "ComputeSkinningPass.h"
#include "renderer/Instance/Mesh/MeshInstance.h"
#include "Shaders/skinning/skinning.hlsli"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ComputeSkinningPass::ComputeSkinningPass() :
        ComputePass("ComputeSkinningPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::CS, 0, 0, RootConstants2DCount);
        rsDesc.addTable(bindlessTable);

        m_computeSkinningRootSignature = device->addRootSignature(rsDesc);
        m_computeSkinningShaderKey.init("system/skinning/skinning.hlsl", "CS_Skinning");
    }

    //--------------------------------------------------------------------------------------
    ComputeSkinningPass::~ComputeSkinningPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_computeSkinningRootSignature);
    }

    //--------------------------------------------------------------------------------------
    void ComputeSkinningPass::clearSkins()
    {
        m_skinnedMeshes.clear();
    }

    //--------------------------------------------------------------------------------------
    void ComputeSkinningPass::addSkins(const core::vector<MeshInstance *> * _skinnedMeshes)
    {
        m_skinnedMeshes.push_back(_skinnedMeshes);
    }

    //--------------------------------------------------------------------------------------
    void ComputeSkinningPass::Setup(const gfx::RenderPassContext & _renderPassContext, double _dt)
    {
        auto * device = Device::get();

        FrameGraphBufferResourceDesc skinningRWBufferDesc;
        skinningRWBufferDesc.elementCount = (4 * 1024 * 1024) >> 2;
        skinningRWBufferDesc.elementSize = 4;

        const auto skinningRWBuffer = _renderPassContext.getFrameGraphID("SkinningRWBuffer");
        createRWBuffer(skinningRWBuffer, skinningRWBufferDesc);
        writeRWBuffer(skinningRWBuffer);
    }

    //--------------------------------------------------------------------------------------
    void ComputeSkinningPass::Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const
    {
        uint offset = 0;

        for (uint j = 0; j < m_skinnedMeshes.size(); ++j)
        {
            const core::vector<MeshInstance*> & list = *m_skinnedMeshes[j];
            for (uint i = 0; i < list.size(); ++i)
            {
                MeshInstance * meshInstance = list[i];
                const MeshModel * meshModel = meshInstance->getMeshModel(Lod::Lod0);
                const MeshGeometry * meshGeo = meshModel->getGeometry();

                //const VertexFormat meshFmt = meshGeo->getVertexFormat();
                //const uint meshFmtSize = getVertexFormatStride(meshFmt);

                const uint vertexCount = meshGeo->getVertexCount();
                const uint vertexSize = meshGeo->getVertexSize();

                meshInstance->clearRuntimeFlag(MeshInstance::RuntimeFlags::SkinLOD0);
                offset += vertexCount * vertexSize;
            }
        }

        //auto size = _renderPassContext.m_view->GetSize();
        //auto threadGroupSize = uint2(POSTPROCESS_THREADGROUP_SIZE_X, POSTPROCESS_THREADGROUP_SIZE_Y);
        //auto threadGroupCount = uint3((size.x + threadGroupSize.x - 1) / threadGroupSize.x, (size.y + threadGroupSize.y - 1) / threadGroupSize.y, 1);
        //
        //_cmdList->setComputeRootSignature(m_computePostProcessRootSignature);
        //_cmdList->setComputeShader(m_computePostProcessShaderKey);
        //
        //u16 src = getRenderTarget(_renderPassContext.getFrameGraphID("Color"))->getTextureHandle();
        //u16 dst = getRWTexture(_renderPassContext.getFrameGraphID("PostProcessUAV"))->getRWTextureHandle();
        //
        //PostProcessConstants postProcess;
        //postProcess.width_height = packUint16(size.xy);
        //postProcess.srv_uav = packUint16(uint2(src, dst));
        //_cmdList->setComputeRootConstants(0, (u32 *)&postProcess, PostProcessConstantsCount);
        //
        //_cmdList->dispatch(threadGroupCount);
    }
}