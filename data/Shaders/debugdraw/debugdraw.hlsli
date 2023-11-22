#pragma once

#include "system/constants.hlsli"
#include "system/packing.hlsli"
#include "system/rootConstants3D.hlsli"

// decl type TypeAlias unimplemented for Vulkan (DXC SPIRV)
//using DebugDrawRootConstants3D = RootConstants3D;
#define DebugDrawRootConstants3D RootConstants3D
#define DebugDrawRootConstants3DCount sizeof(DebugDrawRootConstants3D)/sizeof(u32)

//#ifndef __cplusplus
//DECL_ROOTCONSTANTS(DebugDrawRootConstants3D, rootConstants3D, 0, 0);
//#endif