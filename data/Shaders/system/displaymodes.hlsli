#pragma once

#include "types.hlsli"

//--------------------------------------------------------------------------------------
// Warning! This file is included in C++ headers files. 
// All shader-only code should be added to displaymode.hlsl instead
//--------------------------------------------------------------------------------------

enum
class DisplayMode : uint
{
	None = 0,

    Forward_MatID,
	Forward_VSNormal,
    Forward_VSTangent,
    Forward_VSBinormal,
    Forward_VSColor,
	Forward_UV0,
    Forward_UV1,
    Forward_Albedo,
    Forward_PSNormal,
    Forward_WorldPos,
    Forward_ScreenPos,

    Deferred_GBuffer0_RGB,
    Deferred_GBuffer0_A,
    Deferred_GBuffer1_RGB,
    Deferred_GBuffer1_A,
    Deferred_GBuffer2_RGB,
    Deferred_GBuffer2_A,

    RayTracing_Hit,
    RayTracing_Barycentrics,
    RayTracing_InstanceID,
    RayTracing_GeometryIndex,
    RayTracing_PrimitiveIndex,  

    PostProcess_Depth,
    PostProcess_Stencil,
    PostProcess_LinearDepth,
    PostProcess_WorldPos,
};

enum DisplayFlags : uint
{
    AlbedoMap = 0x00000001,
    NormalMap = 0x00000002
};
