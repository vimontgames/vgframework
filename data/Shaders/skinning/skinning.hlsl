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

        // TODO : skin

        RWByteAddressBuffer dstRWBuffer = getRWBuffer(computeSkinningConstants.getDestHandle());
        uint dstOffset  = computeSkinningConstants.getDestOffset();
        vertex.Store(dstRWBuffer, vtxFmt, vertexID, dstOffset);
    }
}