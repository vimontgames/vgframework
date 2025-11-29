#pragma once

#include "system/instancedata.hlsli"
#include "system/vertex.hlsli"
#include "system/color.hlsli"
#include "system/debugdisplay.hlsli"

//--------------------------------------------------------------------------------------
// Returns interpolated value of vertex attributes at ray intersection position.
// (Only works for indexed geometry)
//--------------------------------------------------------------------------------------
Vertex getRaytracingInterpolatedVertex(GPUInstanceData instanceData, GPUBatchData batchData, uint primitiveIndex, float2 barycentrics)
{
    ByteAddressBuffer ib = getNonUniformBuffer(instanceData.getIndexBufferHandle());
    uint index[3];
    uint triangleStartIndex = primitiveIndex*3 + batchData.getStartIndex();
                    
    if (instanceData.getIndexSize() == 2)
    {
        uint load0 = ib.Load( ((triangleStartIndex & ~1)<<1) + 0);
        uint load1 = ib.Load( ((triangleStartIndex & ~1)<<1) + 4);
        if (triangleStartIndex & 1)
        {
            index[0] = load0 >> 16;
            index[1] = load1 & 0xFFFF;
            index[2] = load1 >> 16;
        }
        else    
        {
            index[0] = load0 & 0xFFFF;
            index[1] = load0 >> 16;
            index[2] = load1 & 0xFFFF;
        }
    }
    else
    {
        index[0] = ib.Load((triangleStartIndex<<2) + 0);
        index[1] = ib.Load((triangleStartIndex<<2) + 4);
        index[2] = ib.Load((triangleStartIndex<<2) + 8);
    }

    ByteAddressBuffer vb = getNonUniformBuffer(instanceData.getVertexBufferHandle());
    uint vbOffset = instanceData.getVertexBufferOffset();
    VertexFormat vertexFormat = instanceData.getVertexFormat();

    Vertex verts[3];
    verts[0].Load(vb, vertexFormat, index[0], vbOffset);
    verts[1].Load(vb, vertexFormat, index[1], vbOffset);
    verts[2].Load(vb, vertexFormat, index[2], vbOffset);

    float3 bary = float3(0, barycentrics);
    bary.x = (1-bary.y-bary.z);
                    
    Vertex vert;
    vert.Interpolate(verts, bary);
        
    return vert;
}

struct MaterialSample
{
    uint matID;
    SurfaceType surfaceType;
    float2 uv0;
    float4 albedo;
};

template <typename QUERY> MaterialSample getRaytracingMaterial(uint instanceID, uint geometryIndex, uint primitiveIndex, float2 triangleBarycentrics, float3 rayT, float3 worldRayOrigin, float3 worldRayDirection, ViewConstants viewConstants)
{
    MaterialSample mat;
    
    // Raytracing should not be view-dependent, it only works for primary ray!
    DisplayMode mode = viewConstants.getDisplayMode();
    DisplayFlags flags = viewConstants.getDisplayFlags();
    
    // Get instance data from raytracing instance ID
    uint instanceDataOffset = instanceID * GPU_INSTANCE_DATA_ALIGNMENT;
    GPUInstanceData instanceData = getBuffer(RESERVEDSLOT_BUFSRV_INSTANCEDATA).Load < GPUInstanceData > (instanceDataOffset);
    float4 instanceColor = instanceData.getInstanceColor();
              
    // Get batch data from raytracing geometry index and load material
    GPUBatchData batchData = instanceData.loadGPUBatchData(instanceDataOffset, geometryIndex);
    GPUMaterialData materialData = batchData.loadGPUMaterialData();
    float4 materialColor = materialData.getAlbedoColor();
             
    // Use barycentrics to get interpolated attribute values
    Vertex vert = getRaytracingInterpolatedVertex(instanceData, batchData, primitiveIndex, triangleBarycentrics);

    float nearDist = viewConstants.getCameraNearFar().x;
    float3 worldPosition = worldRayOrigin + (rayT-nearDist) * worldRayDirection;
    float2 uv0 = materialData.GetUV0(vert.uv[0], vert.uv[1], worldPosition);
                    
    float4 vertexColor = materialData.getVertexColorOut(vert.getColor(), instanceData.getInstanceColor(), flags, mode);
    
    mat.matID = geometryIndex;
    mat.surfaceType = materialData.getSurfaceType();
    mat.uv0 = uv0;
    mat.albedo = materialData.getAlbedo(uv0, vertexColor, flags, mode, true);
        
    #ifdef _TOOLMODE
    switch (mode)
    {
        default:
        case DisplayMode::RayTracing_Instance_WorldPosition:
            mat.albedo.rgb = SRGBToLinear(frac(worldPosition));
        break;
        
        case DisplayMode::RayTracing_Instance_MaterialID:
            mat.albedo.rgb = SRGBToLinear(getMatIDColor(mat.matID));
            break;
                
        case DisplayMode::RayTracing_Geometry_UV0:
            mat.albedo.rgb = SRGBToLinear(float3(mat.uv0, 0));
            break;
        
        case DisplayMode::RayTracing_Material_SurfaceType:
            mat.albedo.rgb = getSurfaceTypeColor(mat.surfaceType).rgb;
            break;
        
        case DisplayMode::RayTracing_Material_Albedo:
            mat.albedo.rgb = mat.albedo.rgb;
            break;
    }
    #endif
    
    return mat;    
}

template <typename Q> MaterialSample getRaytracingCandidateMaterial(Q query, ViewConstants viewConstants)
{
    return getRaytracingMaterial<Q>(query.CandidateInstanceID(), 
                                    query.CandidateGeometryIndex(), 
                                    query.CandidatePrimitiveIndex(),
                                    query.CandidateTriangleBarycentrics(),
                                    query.CandidateTriangleRayT(),
                                    query.WorldRayOrigin(),
                                    query.WorldRayDirection(),
                                    viewConstants);
}

template <typename Q> MaterialSample getRaytracingCommittedMaterial(Q query, ViewConstants viewConstants)
{
    return getRaytracingMaterial<Q>(query.CommittedInstanceID(), 
                                    query.CommittedGeometryIndex(), 
                                    query.CommittedPrimitiveIndex(),
                                    query.CommittedTriangleBarycentrics(),
                                    query.CommittedRayT(),
                                    query.WorldRayOrigin(),
                                    query.WorldRayDirection(),
                                    viewConstants);
}

#if _TOOLMODE
bool IsRaytracingDebugDisplayMode(DisplayMode mode)
{
    switch (mode)
    {
        default:
            return false;
        
        case DisplayMode::RayTracing_Instance_WorldPosition:
        case DisplayMode::RayTracing_Instance_MaterialID:
        case DisplayMode::RayTracing_Geometry_UV0: 
        case DisplayMode::RayTracing_Material_SurfaceType:
        case DisplayMode::RayTracing_Material_Albedo: 
            return true;
    }
}
#endif
