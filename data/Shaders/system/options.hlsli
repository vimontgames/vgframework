#pragma once

#include "types.hlsli"

enum DisplayMode : uint
{
	Default = 0,
    MatID,
	VSNormal,
    VSTangent,
    VSBinormal,
    VSColor,
	UV0,
    UV1,
    Albedo,
    PSNormal,
    WorldPos,
    ScreenPos
};

enum DisplayFlags : uint
{
    AlbedoMap   = 0x00000001,
    NormalMap   = 0x00000002    
};