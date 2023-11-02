#pragma once

#ifdef __cplusplus
#define ENUM(name, type) enum name : vg::core::type
#else
#define ENUM(name, type) enum name : type
#endif

ENUM(DisplayMode, uint)
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
    PSNormal
};

ENUM(DisplayFlags, uint)
{
    AlbedoMap   = 0x00000001,
    NormalMap   = 0x00000002    
};
