#pragma once

#include "quad.hlsli"
#include "constants.hlsli"

struct RootConstants2D
{
    QuadConstants quad;
    uint texID;
};

#define RootConstants2DCount sizeof(RootConstants2D)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants2D, rootConstants2D, 0, 0);
#endif