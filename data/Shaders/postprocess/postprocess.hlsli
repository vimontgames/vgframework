#pragma once

#define POSTPROCESS_THREADGROUP_SIZE_X 16
#define POSTPROCESS_THREADGROUP_SIZE_Y 16

#include "system/constants.hlsli"
#include "system/packing.hlsli"

struct PostProcessConstants
{
    uint width_height;
    uint srv_uav;
};

#define PostProcessConstantsCount sizeof(PostProcessConstants)/sizeof(u32)

#ifndef __cplusplus
DECL_ROOTCONSTANTS(PostProcessConstants, postProcessConstants, 0, 0);
#endif