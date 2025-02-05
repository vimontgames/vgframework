#ifndef _DISPLAY_MODES__HLSLI_
#define _DISPLAY_MODES__HLSLI_

#include "types.hlsli"

//--------------------------------------------------------------------------------------
// Warning! This file is included in C++ headers files. 
// All shader-only code should be added to displaymode.hlsl instead
//--------------------------------------------------------------------------------------

vg_enum_class_global(DisplayMode, uint,
	None = 0,

    Instance_Color,

    Material_Albedo,         
    Material_NormalMap,      
    Material_AmbientOcclusion,
    Material_Roughness,      
    Material_Metalness,   
 
	Geometry_UV0,            
    Geometry_UV1,   
    Geometry_VertexColor,     
	Geometry_VertexNormal,    
    Geometry_VertexTangent,   
    Geometry_VertexBinormal,  
    Geometry_MaterialID,    
   
    Forward_SurfaceType,  
    Forward_WorldPosition,   
    Forward_WorldNormal,    
    Forward_ScreenPos,     

    Environment_Cubemap,
    Environment_IrradianceCubemap,
    Environment_SpecularReflectionCubemap, 

    Deferred_Albedo,
    Deferred_AlbedoAlpha,
    Deferred_Normal,
    Deferred_NormalAlpha,
    Deferred_PBR,
    Deferred_PBRAlpha,
    Deferred_MSAAEdges,

    Lighting_Diffuse,
    Lighting_Specular,
    Lighting_DirectDiffuse,
    Lighting_DirectSpecular,
    Lighting_EnvironmentDiffuse,
    Lighting_EnvironmentSpecular,
    Lighting_EnvironmentSpecularBRDF,
    Lighting_RayCount,

    RayTracing_Committed_Hit,
    RayTracing_Committed_InstanceID,
    RayTracing_Committed_GeometryIndex,
    RayTracing_Committed_PrimitiveIndex,
    RayTracing_Committed_Barycentrics,
    RayTracing_Committed_WorldPosition,

    RayTracing_Attributes_UV0, 
    RayTracing_Attributes_Albedo, 

    PostProcess_Depth,
    PostProcess_Stencil,
    PostProcess_LinearDepth,
    PostProcess_DepthOfField,
    PostProcess_FXAAEdges,
    PostProcess_WorldPos,
    PostProcess_OutlineMask
);

vg_enum_global(DisplayFlags, uint,
    AlbedoMap       = 0x00000001,
    NormalMap       = 0x00000002,
    VertexColor     = 0x00000004,
    MaterialColor   = 0x00000008,
    InstanceColor   = 0x00000010 
);

#endif
