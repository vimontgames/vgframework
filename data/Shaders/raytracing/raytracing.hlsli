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
    
    Vertex vert;

    ByteAddressBuffer vb = getNonUniformBuffer(instanceData.getVertexBufferHandle());
    uint vbOffset = instanceData.getVertexBufferOffset();
    VertexFormat vertexFormat = instanceData.getVertexFormat();

    #if !_PARTICLE // need to figure out the solution for particles
    Vertex verts[3];
    verts[0].Load(vb, vertexFormat, index[0], vbOffset);
    verts[1].Load(vb, vertexFormat, index[1], vbOffset);
    verts[2].Load(vb, vertexFormat, index[2], vbOffset);

    float3 bary = float3(0, barycentrics);
    bary.x = (1-bary.y-bary.z);
    
    vert.Interpolate(verts, bary);
    #endif
        
    return vert;
}

//--------------------------------------------------------------------------------------
// Retrieves material information at ray intersection point.
//--------------------------------------------------------------------------------------
struct MaterialSample
{
    uint matID;
    SurfaceType surfaceType;
    float2 uv0;
    float4 albedo;
};

template <typename QUERY> MaterialSample getRaytracingMaterial(uint instanceID, uint geometryIndex, uint primitiveIndex, float2 triangleBarycentrics, float3 rayT, float3 worldRayOrigin, float rayTMin, float3 worldRayDirection, DisplayFlags _flags, DisplayMode _mode)
{
    MaterialSample mat;
        
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

    float3 worldPosition = worldRayOrigin + (rayT-rayTMin) * worldRayDirection;
    float2 uv0 = materialData.GetUV0(vert.uv[0], vert.uv[1], worldPosition);
                    
    float4 vertexColor = materialData.getVertexColorOut(vert.getColor(), instanceData.getInstanceColor(), _flags, _mode);
    
    mat.matID = geometryIndex;
    mat.surfaceType = materialData.getSurfaceType();
    mat.uv0 = uv0;
    
    mat.albedo = materialData.getAlbedo(uv0, vertexColor, _flags, _mode, true);   

    //#ifdef _TOOLMODE
    //switch (_mode)
    //{
    //    default:
    //    break;
    //
    //    case DisplayMode::RayTracing_Instance_WorldPosition:
    //        mat.albedo.rgb = SRGBToLinear(frac(worldPosition));
    //    break;
    //    
    //    case DisplayMode::RayTracing_Instance_MaterialID:
    //        mat.albedo.rgb = SRGBToLinear(getMatIDColor(mat.matID));
    //        break;
    //            
    //    case DisplayMode::RayTracing_Geometry_UV0:
    //        mat.albedo.rgb = SRGBToLinear(float3(mat.uv0, 0));
    //        break;
    //    
    //    case DisplayMode::RayTracing_Material_SurfaceType:
    //        mat.albedo.rgb = getSurfaceTypeColor(mat.surfaceType).rgb;
    //        break;
    //    
    //    case DisplayMode::RayTracing_Material_Albedo:
    //        mat.albedo.rgb = mat.albedo.rgb;
    //        break;
    //}
    //#endif

    return mat;    
}

//--------------------------------------------------------------------------------------
// Helper functions to get candidate material from ray query
//--------------------------------------------------------------------------------------
template <typename Q> MaterialSample getRaytracingCandidateMaterial(Q query, DisplayFlags _flags = DefaultDisplayFlags, DisplayMode _mode = DisplayMode::None)
{
    return getRaytracingMaterial<Q>(query.CandidateInstanceID(), 
                                    query.CandidateGeometryIndex(), 
                                    query.CandidatePrimitiveIndex(),
                                    query.CandidateTriangleBarycentrics(),
                                    query.CandidateTriangleRayT(),
                                    query.WorldRayOrigin(),
                                    query.RayTMin(),
                                    query.WorldRayDirection(),
                                    _flags,
                                    _mode);
}

//--------------------------------------------------------------------------------------
// Helper functions to get committed material from ray query
template <typename Q> MaterialSample getRaytracingCommittedMaterial(Q query, DisplayFlags _flags = DefaultDisplayFlags, DisplayMode _mode = DisplayMode::None)
{
    return getRaytracingMaterial<Q>(query.CommittedInstanceID(), 
                                    query.CommittedGeometryIndex(), 
                                    query.CommittedPrimitiveIndex(),
                                    query.CommittedTriangleBarycentrics(),
                                    query.CommittedRayT(),
                                    query.WorldRayOrigin(),
                                    query.RayTMin(),
                                    query.WorldRayDirection(),
                                     _flags,
                                    _mode);
}

#if _TOOLMODE
//--------------------------------------------------------------------------------------
// Return 'true' if the current display mode is a raytracing debug display mode
//--------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------
// Raytraced shadow calculation (0.0f = completely shadowed, 1.0f = no shadow)
//--------------------------------------------------------------------------------------
float3 getRaytracedShadow(RaytracingAccelerationStructure _tlas, float3 _worldPos, float3 _worldNormal, float _bias, float3 _lightDir, float _lightFar)
{
    float3 shadow = (float3)1;	
    			
    #if 1
    // Use world normal for bias
	RayDesc ray;
	ray.Origin    = _worldPos + _worldNormal.xyz *  _bias * 10.0f; // TODO: use geometric normal
	ray.Direction = _lightDir;
	ray.TMin      = 0; // OK because we bias origin
	ray.TMax      = _lightFar;
	#else
	// Fixed bias		
	RayDesc ray;
	ray.Origin    = _worldPos;
	ray.Direction = _lightDir;
	ray.TMin      = 0.005;
	ray.TMax      = _lightFar;
	#endif
				
	RayQuery<RAY_FLAG_NONE> query;
	query.TraceRayInline(_tlas, RAY_FLAG_NONE, 0xff, ray);

	while(query.Proceed())
	{
		if (query.CandidateType() == CANDIDATE_NON_OPAQUE_TRIANGLE)
		{				
			MaterialSample mat = getRaytracingCandidateMaterial(query);

            // Ignore decals (doesn't cast shadow)
            if (mat.surfaceType == SurfaceType::AlphaTest)
            {
                if (mat.albedo.a > 0.5f)
                {
                    query.CommitNonOpaqueTriangleHit();
                    break;
                }
            }
            else if (mat.surfaceType == SurfaceType::AlphaBlend && mat.albedo.a > 0.0f)
            {
                shadow *= lerp(1, mat.albedo.rgb, mat.albedo.a);  
            }
		}
	}

	switch(query.CommittedStatus())
	{
		default:
		case COMMITTED_NOTHING:
		break;

		case COMMITTED_TRIANGLE_HIT:
		{
			float dist = query.CommittedRayT();
            shadow = (float3)0;
        }
        break;
    }
    
    return shadow;
}
