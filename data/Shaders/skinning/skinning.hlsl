#include "skinning.hlsli"
#include "system/bindless.hlsli"

[numthreads(SKINNING_THREADGROUP_SIZE_X, 1, 1)]
void CS_Skinning(int dispatchThreadID : SV_DispatchThreadID)
{   
    uint vertexID = dispatchThreadID;

    if (vertexID < computeSkinningConstants.getVertexCount())
    {
        VertexFormat vtxFmt = computeSkinningConstants.getVertexFormat();

        ByteAddressBuffer srcBuffer = getBuffer(computeSkinningConstants.getSourceHandle());
        uint srcOffset = computeSkinningConstants.getSourceOffset();

        Vertex vertex;
        vertex.Load(srcBuffer, vtxFmt, vertexID, srcOffset);

        ByteAddressBuffer skinMatricesBuffer = getBuffer(RESERVEDSLOT_BUFSRV_SKINNINGMATRICES);

        uint boneMatrixOffset = computeSkinningConstants.getMatrixOffset();

        float4x4 boneMatrix = skinMatricesBuffer.Load<float4x4>(boneMatrixOffset + vertex.skinIndices[0] * sizeof(float4x4)) * vertex.skinWeights[0]
                            + skinMatricesBuffer.Load<float4x4>(boneMatrixOffset + vertex.skinIndices[1] * sizeof(float4x4)) * vertex.skinWeights[1]
                            + skinMatricesBuffer.Load<float4x4>(boneMatrixOffset + vertex.skinIndices[2] * sizeof(float4x4)) * vertex.skinWeights[2]
                            + skinMatricesBuffer.Load<float4x4>(boneMatrixOffset + vertex.skinIndices[3] * sizeof(float4x4)) * vertex.skinWeights[3];

        #if _VULKAN
        boneMatrix = transpose(boneMatrix);
        #endif                    

        vertex.pos.xyz = mul(float4(vertex.pos.xyz, 1), boneMatrix).xyz;
        vertex.nrm.xyz = mul(float4(vertex.nrm.xyz, 0), boneMatrix).xyz;
        vertex.tan.xyz = mul(float4(vertex.tan.xyz, 0), boneMatrix).xyz;

        RWByteAddressBuffer dstRWBuffer = getRWBuffer(computeSkinningConstants.getDestHandle());
        uint dstOffset  = computeSkinningConstants.getDestOffset();

        vertex.Store(dstRWBuffer, vtxFmt, vertexID, dstOffset);
    }
}