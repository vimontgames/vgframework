#ifndef _DISPLAY_MODES__HLSLI_
#define _DISPLAY_MODES__HLSLI_

#include "types.hlsli"

//--------------------------------------------------------------------------------------
// Warning! This file is included in C++ headers files. 
// All shader-only code should be added to displaymode.hlsl instead
//--------------------------------------------------------------------------------------

vg_enum_class(DisplayMode, uint,
	None = 0,

    Forward_SurfaceType,
    Forward_MatID,
	Forward_VSNormal,
    Forward_VSTangent,
    Forward_VSBinormal,
    Forward_VSColor,
	Forward_UV0,
    Forward_UV1,
    Forward_Albedo,
    Forward_PSNormal,
    Forward_WorldNormal,
    Forward_WorldPos,
    Forward_ScreenPos,

    Deferred_GBuffer0_Albedo,
    //Deferred_GBuffer0_A,
    Deferred_GBuffer1_Normal,
    //Deferred_GBuffer1_A,
    //Deferred_GBuffer2_OcclusionRoughnessMetalness,
    Deferred_GBuffer2_Occlusion,
    Deferred_GBuffer2_Roughness,
    Deferred_GBuffer2_Metalness,
    Deferred_MSAAEdges,

    Lighting_EnvironmentDiffuse,
    Lighting_EnvironmentSpecular,
    Lighting_DirectLightDiffuse,
    Lighting_DirectLightSpecular,
    Lighting_Diffuse,
    Lighting_Specular,
    Lighting_SpecularBRDF,
    Lighting_RayCount,

    RayTracing_Hit,
    RayTracing_Barycentrics,
    RayTracing_InstanceID,
    RayTracing_GeometryIndex,
    RayTracing_PrimitiveIndex,  

    PostProcess_Depth,
    PostProcess_LinearDepth,
    PostProcess_Stencil,
    PostProcess_FXAAEdges,
    PostProcess_WorldPos
);

vg_enum(DisplayFlags, uint,
    AlbedoMap       = 0x00000001,
    NormalMap       = 0x00000002,
    VertexColor     = 0x00000004,
    MaterialColor   = 0x00000008,
    InstanceColor   = 0x00000010 
);

#endif
