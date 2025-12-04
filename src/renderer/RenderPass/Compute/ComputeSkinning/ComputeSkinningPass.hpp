#include "ComputeSkinningPass.h"
#include "renderer/Instance/Mesh/MeshInstance.h"
#include "renderer/Animation/Skeleton.h"
#include "renderer/RayTracing/RayTracingManager.h"
#include "Shaders/skinning/skinning.hlsli"

namespace vg::renderer
{
    // Buffer for skinning matrices
    static core::uint s_SkinningMatricesBufferSize = 8 * 1024 * 1024;

    // Buffer for skinned vertices
	static core::uint s_ComputeSkinningBufferSize = 128 * 1024 * 1024; 

    //--------------------------------------------------------------------------------------
    ComputeSkinningPass::ComputeSkinningPass() :
        ComputePass("ComputeSkinningPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::CS, 0, 0, ComputeSkinningConstantsCount);
        rsDesc.addTable(bindlessTable);

        m_computeSkinningRootSignature = device->addRootSignature(rsDesc);
        m_computeSkinningShaderKey.init("skinning/skinning.hlsl", "SkinningCS");

        BufferDesc skinningMatricesBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, sizeof(float4x4), s_ComputeSkinningBufferSize / sizeof(float4x4));
        m_skinningMatricesBuffer = device->createBuffer(skinningMatricesBufferDesc, "SkinningMatrices", nullptr, ReservedSlot::SkinningMatricesBufSrv);
    }

    //--------------------------------------------------------------------------------------
    ComputeSkinningPass::~ComputeSkinningPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_computeSkinningRootSignature);
        VG_SAFE_RELEASE(m_skinningMatricesBuffer);
    }

    //--------------------------------------------------------------------------------------
    // Returns the skins to process. It can be:
    // - Only visible skins
    // - All world skins
    //--------------------------------------------------------------------------------------
    core::span<MeshInstance *> ComputeSkinningPass::getSkinnedMeshes() const
    {
        RendererOptions * options = RendererOptions::get();
        if (options->isRayTracingEnabled() && options->getRayTracingTLASMode() == TLASMode::PerWorld)
        {
            // skin all skinned meshes in the world
            return RayTracingManager::get()->getSkinnedMeshInstances();
        }
        else
        {
            // Skin only visible skins
            return Renderer::get()->getSharedCullingJobOutput()->m_skins;
        }
    }

    //--------------------------------------------------------------------------------------
    // Estimate returns a cost of 1 per instance to update
    //--------------------------------------------------------------------------------------
    core::u64 ComputeSkinningPass::GetCostEstimate(const RenderPassContext & _renderContext) const
    {
        return getSkinnedMeshes().size();
    }

    //--------------------------------------------------------------------------------------
    void ComputeSkinningPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        auto * device = Device::get();

        FrameGraphBufferResourceDesc skinningRWBufferDesc;
        skinningRWBufferDesc.elementCount = s_ComputeSkinningBufferSize >> 2;
        skinningRWBufferDesc.elementSize = 4;

        const auto skinningRWBuffer = "SkinningRWBuffer";
        createRWBuffer(skinningRWBuffer, skinningRWBufferDesc);
        writeRWBuffer(skinningRWBuffer);
    }

    //--------------------------------------------------------------------------------------
    void ComputeSkinningPass::BeforeAll(const RenderPassContext & _renderContext)
    {
        VG_PROFILE_CPU("Skinning");
        const auto & skinnedMeshes = getSkinnedMeshes();

        // Compute bones upload size
        uint dstVertOffset = 0;
        uint totalBoneCount = 0;

        Buffer * dstBuffer = getRWBuffer("SkinningRWBuffer");

        for (uint i = 0; i < skinnedMeshes.size(); ++i)
        {
            MeshInstance * meshInstance = skinnedMeshes[i];

            const MeshModel * meshModel = meshInstance->getMeshModel(Lod::Lod0);
            const MeshGeometry * meshGeo = meshModel->getGeometry();
            const Skeleton * skeleton = meshInstance->getInstanceSkeleton();
            const uint boneCount = skeleton->getBoneCount();
            const uint vertexCount = meshGeo->getVertexCount();
            const uint vertexSize = meshGeo->getVertexSize();

            meshInstance->setInstanceVertexBuffer(dstBuffer, dstVertOffset);
            meshInstance->removeAtomicFlags(GraphicInstance::AtomicFlags::SkinList);

            dstVertOffset += vertexCount * vertexSize;
            totalBoneCount += boneCount;
        }

        m_mapSize = totalBoneCount * sizeof(float4x4);
    }

    //--------------------------------------------------------------------------------------
    void ComputeSkinningPass::Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const
    {
        uint dstMatOffset = 0;

        const auto & skinnedMeshes = getSkinnedMeshes();

        size_t mapSizeInBytes = m_mapSize;

        // Upload bones matrices
        if (mapSizeInBytes > 0)
        {
            u8 * skinningMatrices = (u8 *)_cmdList->map(m_skinningMatricesBuffer, mapSizeInBytes).data;
            {
                for (uint i = 0; i < skinnedMeshes.size(); ++i)
                {
                    const MeshInstance * meshInstance = skinnedMeshes[i];
                    const MeshModel * meshModel = meshInstance->getMeshModel(Lod::Lod0);
                    const Skeleton * skeleton = meshInstance->getInstanceSkeleton();
                    const uint boneCount = skeleton->getBoneCount();
                    const auto & nodes = skeleton->getNodes();
                    const auto & boneIndices = skeleton->getBoneIndices();
                    const auto & boneMatrices = skeleton->getBoneMatrices();
                    VG_ASSERT(dstMatOffset + boneCount * sizeof(float4x4) < s_SkinningMatricesBufferSize, "[Skinning] Buffer is too small to store skinning matrices");

                    for (uint b = 0; b < boneCount; ++b)
                    {
                        uint index = boneIndices[b];
                        const MeshImporterNode & bone = nodes[index];
                        float4x4 boneMatrix = mul(bone.node_to_world, boneMatrices[b]);
                        (float4x4 &)(skinningMatrices[dstMatOffset + b * sizeof(float4x4)]) = transpose(boneMatrix);
                    }

                    dstMatOffset += boneCount * sizeof(float4x4);
                }
            }
            _cmdList->unmap(m_skinningMatricesBuffer); // TODO: pass the size actually written so that we don't update the whole buffer 
        }

        // Skin vertices
        dstMatOffset = 0;
        uint dstVertOffset = 0;

        for (uint i = 0; i < skinnedMeshes.size(); ++i)
        {
            const MeshInstance * meshInstance = skinnedMeshes[i];
            const MeshModel * meshModel = meshInstance->getMeshModel(Lod::Lod0);
            const MeshGeometry * meshGeo = meshModel->getGeometry();
            const Skeleton * skeleton = meshInstance->getInstanceSkeleton();
            const uint boneCount = skeleton->getBoneCount();

            const VertexFormat meshFmt = meshGeo->getVertexFormat();
            //const uint meshFmtSize = getVertexFormatStride(meshFmt);
            const uint vertexCount = meshGeo->getVertexCount();
            const uint vertexSize = meshGeo->getVertexSize();

            VG_ASSERT(dstVertOffset + vertexCount * vertexSize < s_ComputeSkinningBufferSize, "[Skinning] Buffer is too small to store skinned vertices");
            if (dstVertOffset + vertexCount * vertexSize < s_ComputeSkinningBufferSize)
            {
                _cmdList->setComputeRootSignature(m_computeSkinningRootSignature);
                _cmdList->setComputeShader(m_computeSkinningShaderKey);

                auto threadGroupSize = uint1(SKINNING_THREADGROUP_SIZE_X);
                auto threadGroupCount = uint3((vertexCount + threadGroupSize.x - 1) / threadGroupSize.x, 1, 1);

                BindlessBufferHandle src = meshGeo->getVertexBuffer()->getBufferHandle();
                uint srcOffset = meshGeo->getVertexBufferOffset();

                Buffer * dstBuffer = getRWBuffer("SkinningRWBuffer");
                BindlessRWBufferHandle dst = dstBuffer->getRWBufferHandle();

                // TODO : Pass vertex buffer to skin and destination buffer with offset
                ComputeSkinningConstants skinningRootConstants;
                skinningRootConstants.setVertexFormat(meshFmt);
                skinningRootConstants.setMatrixOffset(dstMatOffset);
                skinningRootConstants.setVertexCount(vertexCount);
                skinningRootConstants.setSource(src, srcOffset);
                skinningRootConstants.setDest(dst, dstVertOffset);
                _cmdList->setComputeRootConstants(0, (u32 *)&skinningRootConstants, ComputeSkinningConstantsCount);

                _cmdList->dispatch(threadGroupCount);

                // Store skinned VB and offset in mesh
                VG_ASSERT(dstBuffer == meshInstance->getInstanceVertexBuffer());
                VG_ASSERT(dstVertOffset == meshInstance->getInstanceVertexBufferOffset());

                dstVertOffset += vertexCount * vertexSize;
                dstMatOffset += boneCount * sizeof(float4x4);
            }
        }
        VG_ASSERT(mapSizeInBytes == dstMatOffset);
    }
}